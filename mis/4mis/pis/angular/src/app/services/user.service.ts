import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Observable, BehaviorSubject } from 'rxjs';

import { User } from '../models';
import { serviceConfiguration, userRolesDb } from '../config';
import { AuthenticationService } from 'src/app/services/authentication.service';
import { AlertService } from './alert.service';

@Injectable({
  providedIn: 'root'
})
export class UserService {

  private _employees: BehaviorSubject<any> = new BehaviorSubject(null);
  private _employeesCache: any = [];
  private _loadingEmployees: boolean;
  private _userData: BehaviorSubject<User> = new BehaviorSubject<User>(null);
  private _userDataCache: User;

  private _customers: BehaviorSubject<Array<User>> = new BehaviorSubject(null);
  private _customersCache: Array<User> = [];
  private _loadingCustomers: boolean = false;

  constructor(
    private _http: HttpClient,
    private _authService: AuthenticationService,
    private _alertService: AlertService
  ) { }

  public get loadingEmployees(): boolean {
    return this._loadingEmployees;
  }
  
  public get loadingCustomers(): boolean {
    return this._loadingCustomers;
  }

  private get authHeader(): object {
    if (this._authService.loggedUser) {
      return { headers: new HttpHeaders().set('Authorization', this._authService.loggedUser.token) };
    } else {
      return {};
    }
  }
  public get userIsAuthorized(): boolean {
    return this._authService.loggedUser !== null && this._authService.loggedUser.role !== 'customer';
  }

  public postEmployee(employee: User): void {
    this._http.post<any>(serviceConfiguration.employees.apiPostEmployee, employee.apiNewEmployee, this.authHeader).subscribe(
      _ => {
        this.getEmployees();
        this._alertService.success(employee.name + ' ' + employee.surname + ' byl úspěšně přidán mezi zaměstnance.');
      },
      error => {
        console.log(error);
        if (error.statusText === 'Conflict') {
          this._alertService.error('Zaměstnanec se zadaným e-mailem již existuje!');
        } else {
          this._alertService.error('Zaměstnance se nepodařilo přidat: ' + error.statusText);
        }
      }
    );
  }

  public updateEmployee(employee: User, profile: boolean = false): void {
    const url = serviceConfiguration.employees.apiPutEmployee + employee.userId;
    this._http.put<any>(url, employee.apiUpdateEmployee, this.authHeader).subscribe(
      _ => {
        if (profile) {
          this._alertService.success('Profil byl úspěšně změněn.', '/login');
          this._authService.logout();
        } else {
          this.getEmployees();
          this._alertService.success('Změny zaměstance byly úspěšně uloženy.');
        }
      },
      error => {
        this._alertService.error('Změny se nepodařilo provést: ' + error.statusText);
      }
    );
  }

  public getEmployee(employeeId: number): Observable<User> {
    this._http.get(serviceConfiguration.employees.apiGetEmployee + String(employeeId), this.authHeader).subscribe(
      response => {
        this._userDataCache = this.buildUser(response);
        this._userData.next(this._userDataCache);
      },
      error => {
        this._alertService.error('Nepodařilo se načíst informace o zaměstnanci: ' + error.statusText);
      }
    );
    return this._userData.asObservable();
  }

  public getEmployees(): Observable<Array<User>> {
    if (this.userIsAuthorized) {
      this._loadingEmployees = true;
      this._http.get(serviceConfiguration.employees.apiList, this.authHeader).subscribe(
        (response: Array<any>) => {
          this._employeesCache = this.buildUsersFromData(response);
          this._employees.next(this._employeesCache);
          this._loadingEmployees = false;
        },
        error => {
          this._alertService.error('Nepodařilo se načíst zaměstnance: ' + error.statusText);
          this._loadingEmployees = false;
        }
      );
    }

    return this._employees.asObservable();
  }


  public getCustomers(): Observable<Array<User>> {
    if (this.userIsAuthorized) {
      this._loadingCustomers = true;
      this._http.get(serviceConfiguration.customers.apiList, this.authHeader).subscribe(
        (response: Array<any>) => {
          this._customersCache = this.buildUsersFromData(response);
          this._customers.next(this._customersCache);
          this._loadingCustomers = false;
        },
        error => {
          this._alertService.error('Nepodařilo se načíst zákazníky: ' + error.statusText);
          this._loadingCustomers = false;
        }
      );
    }

    return this._customers.asObservable();
  }

  
  public postCustomer(customer: User): void {
    this._http.post<any>(serviceConfiguration.customers.apiPostCustomer, customer.apiNewEmployee, this.authHeader).subscribe(
      _ => {
        this.getCustomers();
        this._alertService.success(customer.name + ' ' + customer.surname + ' byl úspěšně přidán mezi zákazníky.');
      },
      error => {
        console.log(error);
        if (error.statusText === 'Conflict') {
          this._alertService.error('Uživatel se zadaným e-mailem již existuje!');
        } 
        else {
          this._alertService.error('Uživatele se nepodařilo přidat: ' + error.statusText);
        }
      }
    );
  }

  public updateCustomer(customer: User, profile: boolean = false): void {
    const url = serviceConfiguration.customers.apiPutCustomer + customer.userId;
    this._http.put<any>(url, customer.apiUpdateEmployee, this.authHeader).subscribe(
      _ => {
        if (profile) {
          this._alertService.success('Profil byl úspěšně změněn.', '/login');
          this._authService.logout();
        } 
        else {
          this.getCustomers();
          this._alertService.success('Změny zákazníka byly úspěšně uloženy.');
        }
      },
      error => {
        this._alertService.error('Změny se nepodařilo provést: ' + error.statusText);
      }
    );
  }

  private buildUsersFromData(data: Array<any>): any {
    if (!data) { return []; }
    const employees: Array<User> = [];
    data.forEach(
      e => {
        employees.push(this.buildUser(e));
      }
    );
    return employees;
  }

  private buildUser(e: any): User {
    const employee: User = new User();
    for (const key in e) {
      if (e.hasOwnProperty(key)) {
        employee[key] = e[key];
      }
    }
    return employee;
  }
}
