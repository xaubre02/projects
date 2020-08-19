import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Observable, BehaviorSubject } from 'rxjs';

import { MaterialOrder, Material } from '../models';
import { serviceConfiguration } from '../config';
import { AuthenticationService } from 'src/app/services/authentication.service';
import { AlertService } from './alert.service';
import { userRolesDb } from '../config';

@Injectable({
  providedIn: 'root'
})
export class MaterialOrderService {

  private _materialOrders: BehaviorSubject<Array<MaterialOrder>> = new BehaviorSubject([]);
  private _materialOrdersCache: Array<MaterialOrder> = [];
  private _loadingMaterialOrders: boolean = false;

  constructor(
    private _http: HttpClient,
    private _authService: AuthenticationService,
    private _alertService: AlertService
  ) { }

  public get loadingMaterialOrders(): boolean {
    return this._loadingMaterialOrders;
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
  private get userMaterialStoreman(): boolean {
    if (this._authService.loggedUser) {
      return this._authService.loggedUser.role == userRolesDb.materialstoreman;
    }
    return false;
  }


  public getMaterialOrders(): Observable<Array<MaterialOrder>> {
    if (this.userManager || this.userMaterialStoreman) {
      this._loadingMaterialOrders = true;
      this._http.get(serviceConfiguration.materialOrders.apiList, this.authHeader).subscribe(
        (response: Array<any>) => {
          this._materialOrdersCache = this.buildOrdersFromData(response);
          this._materialOrders.next(this._materialOrdersCache);
          this._loadingMaterialOrders = false;
        },
        error => {
          console.log(error);
          this._alertService.error('Nepodařilo se načíst objednávky: ' + error.statusText);
          this._loadingMaterialOrders = false;
        }
      );
    }

    return this._materialOrders.asObservable();
  }

  
  public getMaterialOrdersByState(delivered: boolean): Observable<Array<MaterialOrder>> {
    if (this.userManager || this.userMaterialStoreman) {
      this._loadingMaterialOrders = true;
      const url: string = serviceConfiguration.materialOrders.apiState.replace('%STATE%', delivered.toString());

      this._http.get(url, this.authHeader).subscribe(
        (response: Array<any>) => {
          this._materialOrdersCache = this.buildOrdersFromData(response);
          this._materialOrders.next(this._materialOrdersCache);
          this._loadingMaterialOrders = false;
        },
        error => {
          console.log(error);
          this._alertService.error('Nepodařilo se načíst objednávky: ' + error.statusText);
          this._loadingMaterialOrders = false;
        }
      );
    }

    return this._materialOrders.asObservable();
  }

  public createMaterialOrder(order: MaterialOrder): void {
    if (this.userManager) {
      this._http.post<any>(serviceConfiguration.materialOrders.apiOrderPost, order.apiNewOrder, this.authHeader).subscribe(
        _ => {
          this.getMaterialOrders();
          this._alertService.success('Objednávka byla úspěšně vytvořena.');
        },
        error => {
          console.log(error);
          this._alertService.error('Chyba při vytváření objednávky ' + error.statusText);
        }
      );
    }
  }

  public acceptMaterialOrder(order: MaterialOrder): void {
    if (this.userManager || this.userMaterialStoreman) {
      let orderId: string = order.orderId.toString();
      const url: string = serviceConfiguration.materialOrders.apiOrderAccept.replace('%ORDER%', orderId);

      this._http.put<any>(url, {}, this.authHeader).subscribe(
        _ => {
          this.getMaterialOrders();
          this._alertService.success('Objednávka "' + orderId + '" byla úspěšně přijata.');
        },
        error => {
          console.log(error);
          this._alertService.error('Nastala chyba při přijetí objednávky "' + orderId + '".');
        }
      );
    }
  }

  public cancelMaterialOrder(order: MaterialOrder): void {
    if (this.userManager) {
      let orderId: string = order.orderId.toString();
      const url: string = serviceConfiguration.materialOrders.apiOrderCancel.replace('%ORDER%', orderId);

      this._http.delete<any>(url, this.authHeader).subscribe(
        _ => {
          this.getMaterialOrders();
          this._alertService.success('Objednávka "' + orderId + '" byla zrušena.');
        },
        error => {
          console.log(error);
          this._alertService.error('Objednávku "' + orderId + '" se nepodařilo zrušit: ' + error.statusText);
        }
      );
    }
  }


  private buildOrdersFromData(data: Array<any>): any {
    let orders: Array<MaterialOrder> = [];
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

  private buildOrder(o: any): MaterialOrder {
    let order: MaterialOrder = new MaterialOrder();
    // nested json objects
    if (o.materials && o.materials.length) {
      let materialsArr = new Array<Material>();
      o.materials.forEach(
        m => {
          let material = new Material();
          for (let key in m) {
            material[key] = m[key];
          }
          materialsArr.push(material);
        }
      );
      o.materials = materialsArr;
    }

    for (const key in o) {
      if (o.hasOwnProperty(key)) {
        order[key] = o[key];
      }
    }
    return order;
  }
}
