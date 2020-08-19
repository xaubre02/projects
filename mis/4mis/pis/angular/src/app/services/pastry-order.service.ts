import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Observable, BehaviorSubject } from 'rxjs';

import { PastryOrder, Pastry, User } from '../models';
import { serviceConfiguration } from '../config';
import { AuthenticationService } from 'src/app/services/authentication.service';
import { AlertService } from './alert.service';
import { userRolesDb } from '../config';

@Injectable({
  providedIn: 'root'
})
export class PastryOrderService {

  private _pastryOrders: BehaviorSubject<Array<PastryOrder>> = new BehaviorSubject([]);
  private _pastryOrdersCache: Array<PastryOrder> = [];
  private _loadingPastryOrders: boolean = false;

  constructor(
    private _http: HttpClient,
    private _authService: AuthenticationService,
    private _alertService: AlertService
  ) { }

  public get loadingPastryOrders(): boolean {
    return this._loadingPastryOrders;
  }

  private get authHeader(): object {
    if (this._authService.loggedUser) {
      return { headers: new HttpHeaders().set('Authorization', this._authService.loggedUser.token) };
    } 
    else {
      return {};
    }
  }

  private get userManager(): boolean {
    if (this._authService.loggedUser) {
      return this._authService.loggedUser.role == userRolesDb.manager;
    }
    return false;
  }
  private get userPastryStoreman(): boolean {
    if (this._authService.loggedUser) {
      return this._authService.loggedUser.role == userRolesDb.pastrystoreman;
    }
    return false;
  }
  private get userCustomer(): boolean {
    if (this._authService.loggedUser) {
      return this._authService.loggedUser.role == userRolesDb.customer;
    }
    return false;
  }

  public getPastryOrders(): Observable<Array<PastryOrder>> {
    if (this.userManager || this.userPastryStoreman) {
      this._loadingPastryOrders = true;
      this._http.get(serviceConfiguration.pastryOrders.apiList, this.authHeader).subscribe(
        (response: Array<any>) => {
          this._pastryOrdersCache = this.buildOrdersFromData(response);
          this._pastryOrders.next(this._pastryOrdersCache);
          this._loadingPastryOrders = false;
        },
        error => {
          console.log(error);
          this._alertService.error('Nepodařilo se načíst objednávky: ' + error.statusText);
          this._loadingPastryOrders = false;
        }
      );
    }
    else if (this.userCustomer) {
      this._loadingPastryOrders = true;
      const url: string = serviceConfiguration.pastryOrders.apiCustomer.replace('%USER%', this._authService.loggedUser.userId.toString());

      this._http.get(url, this.authHeader).subscribe(
        (response: Array<any>) => {
          this._pastryOrdersCache = this.buildOrdersFromData(response);
          this._pastryOrders.next(this._pastryOrdersCache);
          this._loadingPastryOrders = false;
        },
        error => {
          console.log(error);
          this._alertService.error('Nepodařilo se načíst objednávky: ' + error.statusText);
          this._loadingPastryOrders = false;
        }
      );
    }

    return this._pastryOrders.asObservable();
  }

  public getPastryOrdersByState(state: string): Observable<Array<PastryOrder>> {
    if (this.userManager || this.userPastryStoreman || this.userCustomer) {
      this._loadingPastryOrders = true;
      const url: string = serviceConfiguration.pastryOrders.apiState.replace('%STATE%', state);

      this._http.get(url, this.authHeader).subscribe(
        (response: Array<any>) => {
          this._pastryOrdersCache = this.buildOrdersFromData(response);
          this._pastryOrders.next(this._pastryOrdersCache);
          this._loadingPastryOrders = false;
        },
        error => {
          console.log(error);
          this._alertService.error('Nepodařilo se načíst objednávky: ' + error.statusText);
          this._loadingPastryOrders = false;
        }
      );
    }

    return this._pastryOrders.asObservable();
  }

  public createPastryOrder(order: PastryOrder): void {
    if (this.userManager || this.userCustomer) {
      this._http.post<any>(serviceConfiguration.pastryOrders.apiOrderPost, order.apiNewOrder, this.authHeader).subscribe(
        _ => {
          this.getPastryOrders();
          this._alertService.success('Objednávka byla úspěšně vytvořena.');
        },
        error => {
          console.log(error);
          this._alertService.error('Chyba při vytváření objednávky ' + error.statusText);
        }
      );
    }
  }

  public updatePastryOrder(order: PastryOrder, deliver: boolean = false): void {
    if (this.userManager || this.userPastryStoreman) {
      let orderId: string = order.orderId.toString();
      const url: string = serviceConfiguration.pastryOrders.apiOrderPut.replace('%ORDER%', orderId);

      this._http.put<any>(url, order.apiEditOrder, this.authHeader).subscribe(
        _ => {
          this.getPastryOrders();
          if (deliver) {
            this._alertService.success('Objednávka "' + orderId + '" byla úspěšně předána zákazníkovi.');
          }
          else {
            this._alertService.success('Změny objednávky "' + orderId + '" byly úspěšně uloženy.');
          }
        },
        error => {
          console.log(error);
          if (deliver) {
            this._alertService.error('Objednávku "' + orderId + '" nyní nelze předat. Na skladě není pravděpodobně dostatek pečiva.');
          }
          else {
            this._alertService.error('Chyba při úpravě objednávky "' + orderId + '": ' + error.statusText);
          }
        }
      );
    }
  }

  public cancelPastryOrder(order: PastryOrder): void {
    if (this.userManager || this.userCustomer) {
      let orderId: string = order.orderId.toString();
      const url: string = serviceConfiguration.pastryOrders.apiOrderCancel.replace('%ORDER%', orderId);

      this._http.put<any>(url, {}, this.authHeader).subscribe(
        _ => {
          this.getPastryOrders();
          this._alertService.success('Objednávka "' + orderId + '" byla zrušena.');
        },
        error => {
          console.log(error);
          this._alertService.error('Objednávku "' + orderId + '" již není možné zrušit: ' + error.statusText);
        }
      );
    }
  }

  private buildOrdersFromData(data: Array<any>): any {
    let orders: Array<PastryOrder> = [];
    if (!data) { 
      return orders; 
    }

    data.forEach(
      o => {
        const material = this.buildOrder(o);
        orders.push(material);
      }
    );
    return orders;
  }

  private buildOrder(o: any): PastryOrder {
    let order: PastryOrder = new PastryOrder();

    // nested json objects
    if (o.items && o.items.length) {
      let itemsArr = new Array<Pastry>();
      o.items.forEach(
        i => {
          let item = new Pastry();
          for (let key in i) {
            item[key] = i[key];
          }
          itemsArr.push(item);
        }
      );
      o.items = itemsArr;
    }
    if (o.customer) {
      let customer = new User();
      for (let key in o.customer) {
        customer[key] = o.customer[key];
      }
      o.customer = customer;
    }

    for (const key in o) {
      if (o.hasOwnProperty(key)) {
        order[key] = o[key];
      }
    }
    return order;
  }
}
