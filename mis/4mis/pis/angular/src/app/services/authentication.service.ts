import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { BehaviorSubject, Observable } from 'rxjs';
import { map } from 'rxjs/operators';

import { AlertService } from './alert.service';
import { User } from '../models';
import { serviceConfiguration } from '../config';

@Injectable({
  providedIn: 'root'
})
export class AuthenticationService {
  private storageKey: string;
  private userSubject: BehaviorSubject<User>;
  private _registering: boolean;
  private _userData: BehaviorSubject<User>;
  private _userDataCache: User;

  public constructor(
    private _http: HttpClient,
    private _alertService: AlertService
  ) {
    this.storageKey = 'loggedUser';
    this.userSubject = new BehaviorSubject<User>(JSON.parse(localStorage.getItem(this.storageKey)));
    this._userData = new BehaviorSubject<User>(null);
  }

  public get observableUser(): Observable<User> { return this.userSubject.asObservable(); }
  public get loggedUser(): User                 { return this.userSubject.value; }
  public get registering(): boolean             { return this._registering; }

  private get authHeader(): object {
    if (this.loggedUser) {
      return { headers: new HttpHeaders().set('Authorization', this.loggedUser.token) };
    } else {
      return {};
    }
  }

  public login(username: string, password: string): Observable<User> {
    return this._http.post<any>(serviceConfiguration.users.apiLogin, { email: username, password: password })
      .pipe(map(response => {
        // login successful if there's a jwt token in the response
        if (response && response.token) {
          localStorage.setItem(this.storageKey, JSON.stringify(response));
          this.userSubject.next(response);
        }
        return response;
      }));
  }

  public logout(): void {
    localStorage.removeItem(this.storageKey);
    this.userSubject.next(null);
  }

  public register(user: User) {
    this._registering = true;
    this._http.post<any>(serviceConfiguration.customers.apiPostCustomer, user.apiNewUser).subscribe(
      _ => {
        this._alertService.success('Registrace proběhla v pořádku.', '/login');
        this._registering = false;
      },
      error => {
        if (error.statusText === 'Conflict') {
          this._alertService.error('Uživatel se zadanou e-mailovou adresou již existuje!');
        } else {
          this._alertService.error('Při registraci nastala chyba: ' + error.statusText);
        }
        this._registering = false;
      }
    );
  }

  public getUser(userId: number): Observable<User> {
    this._http.get(serviceConfiguration.customers.apiGetCustomer + String(userId), this.authHeader).subscribe(
      response => {
        this._userDataCache = this.buildUserFromData(response);
        this._userData.next(this._userDataCache);
      },
      error => {
        this._alertService.error('Nepodařilo se načíst informace o uživateli: ' + error.statusText);
      }
    );
    return this._userData.asObservable();
  }

  public editUser(user: User): void {
    const url = serviceConfiguration.customers.apiPutCustomer + user.userId;
    this._http.put<any>(url, user.apiEditCustomer, this.authHeader).subscribe(
      _ => {
        this._alertService.success('Uživatelský profil byl úspěšně změněn.', '/login');
        this.logout();
      },
      error => {
        this._alertService.error('Změny se nepodařilo provést: ' + error.statusText);
      }
    );
  }

  private buildUserFromData(u: any): User {
    const user: User = new User();
    for (const key in u) {
      if (u.hasOwnProperty(key)) {
        user[key] = u[key];
      }
    }
    return user;
  }
}
