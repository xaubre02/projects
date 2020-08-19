import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { DatePipe } from '@angular/common';
import { Observable, BehaviorSubject } from 'rxjs';

import { ProductionPlan, Pastry, Material } from '../models';
import { serviceConfiguration } from '../config';
import { AuthenticationService } from 'src/app/services/authentication.service';
import { AlertService } from './alert.service';

@Injectable({
  providedIn: 'root'
})
export class ProductionPlanService {

  private _productionPlans: BehaviorSubject<Array<ProductionPlan>> = new BehaviorSubject([]);
  private _productionPlansCache: Array<ProductionPlan> = [];
  
  private _loadingProductionPlans: boolean = false;
  private _loadingProductionPlansToday: boolean = false;
  private _loadingProductionPlanProposal: boolean = false;

  constructor(
    private _http: HttpClient,
    private _authService: AuthenticationService,
    private _alertService: AlertService,
    private _datePipe: DatePipe
  ) { }

  public get loadingProductionPlans(): boolean {
    return this._loadingProductionPlans;
  }
  public get loadingProductionPlansToday(): boolean {
    return this._loadingProductionPlansToday;
  }
  public get loadingProductionPlanProposal(): boolean {
    return this._loadingProductionPlanProposal;
  }

  private get authHeader(): object {
    if (this._authService.loggedUser) {
      return { headers: new HttpHeaders().set('Authorization', this._authService.loggedUser.token) };
    } 
    else {
      return {};
    }
  }

  public getProductionPlans(): Observable<Array<ProductionPlan>> {
    this._loadingProductionPlans = true;
    this._http.get(serviceConfiguration.productionPlans.apiList, this.authHeader).subscribe(
      (response: Array<any>) => {
        this._productionPlansCache = this.buildPlansFromData(response);
        this._productionPlans.next(this._productionPlansCache);
        this._loadingProductionPlans = false;
      },
      error => {
        console.log(error);
        this._alertService.error('Nepodařilo se načíst výrobní plány: ' + error.statusText);
        this._loadingProductionPlans = false;
      }
    );

    return this._productionPlans.asObservable();
  }

  public getTodayProductionPlan(): Observable<ProductionPlan> {
    let todayPlan: BehaviorSubject<ProductionPlan> = new BehaviorSubject(null);
    let today: Date = new Date();
    let todayFormatted: string = this._datePipe.transform(today, 'yyyy-MM-dd');;
    let url: string = serviceConfiguration.productionPlans.apiPlanGetDate.replace('%DATE%', todayFormatted);

    this._loadingProductionPlansToday = true;
    this._http.get(url, this.authHeader).subscribe(
      response => {
        todayPlan.next(this.buildPlan(response));
        this._loadingProductionPlansToday = false;
      },
      error => {
        console.log(error);
        if (error.status == 404) {
          console.log('Pro dnešní den nebyl nalezen žádný výrobní plán.');
        }
        else {
          this._alertService.error('Nepodařilo se načíst výrobní plán pro dnešní den: ' + error.statusText);
        }
        todayPlan.next(null);
        this._loadingProductionPlansToday = false;
      }
    );

    return todayPlan.asObservable();
  }

  public getProposalProductionPlan(plan: ProductionPlan): Observable<ProductionPlan> {
    let proposalPlan: BehaviorSubject<ProductionPlan> = new BehaviorSubject(null);
    let url: string = serviceConfiguration.productionPlans.apiPlanCreate.replace('%DATE%', plan.productionDate);

    this._loadingProductionPlanProposal = true;
    this._http.get(url, this.authHeader).subscribe(
      response => {
        this.getProductionPlans();
        this.getTodayProductionPlan();
        proposalPlan.next(this.buildPlan(response));
        this._loadingProductionPlanProposal = false;
      },
      error => {
        console.log(error);
        this._alertService.error('Nepodařilo se načíst výrobní plán pro dnešní den: ' + error.statusText);
        this._loadingProductionPlanProposal = false;
      }
    );

    return proposalPlan.asObservable();
  }

  public putProductionPlan(plan: ProductionPlan): void {
    let dateFormatted: string = plan.productionDateFormattedCZ;
    let url: string = serviceConfiguration.productionPlans.apiPlanPut.replace('%PLAN%', plan.planId.toString());

    this._http.put<any>(url, plan.apiEditPlan, this.authHeader).subscribe(
      _ => {
        this.getProductionPlans();
        this.getTodayProductionPlan();
        this._alertService.success('Výrobní plán na ' + dateFormatted + ' byl úspěšně upraven.');
      },
      error => {
        console.log(error);
        if (error.status == 404) {
          this._alertService.error('Chyba - výrobní plán na ' + dateFormatted + ' již existuje.');
        }
        else {
          this._alertService.error('Chyba při úpravě výrobního plánu na ' + dateFormatted + ' - ' + error.statusText);
        }
      }
    );
  }

  public postProductionPlan(plan: ProductionPlan): void {
    let dateFormatted: string = plan.productionDateFormattedCZ;

    this._http.post<any>(serviceConfiguration.productionPlans.apiPlanPost, plan.apiNewPlan, this.authHeader).subscribe(
      _ => {
        this.getProductionPlans();
        this.getTodayProductionPlan();
        this._alertService.success('Výrobní plán na ' + dateFormatted + ' byl úspěšně vytvořen.');
      },
      error => {
        console.log(error);
        if (error.status == 404) {
          this._alertService.error('Chyba - výrobní plán na ' + dateFormatted + ' již existuje. Využijte možnost jeho úpravy.');
        }
        else {
          this._alertService.error('Chyba při vytváření výrobního plánu na ' + dateFormatted + ' - ' + error.statusText);
        }
      }
    );
  }

  // method for building array of production plans
  private buildPlansFromData(data: Array<any>): any {
    let plans: Array<ProductionPlan> = [];
    if (!data) { 
      return plans; 
    }

    data.forEach(
      p => {
        const plan = this.buildPlan(p);
        plans.push(plan);
      }
    );
    return plans;
  }

  // method for building a production plan
  private buildPlan(p: any): ProductionPlan {
    let plan: ProductionPlan = new ProductionPlan();

    // nested json objects
    if (p.items && p.items.length) {
      let itemsArr = new Array<Pastry>();
      p.items.forEach(
        i => {
          let item = new Pastry();
          for (let key in i) {
            item[key] = i[key];
          }
          itemsArr.push(item);
        }
      );
      p.items = itemsArr;
    }

    if (p.materials && p.materials.length) {
      let materialsArr = new Array<Material>();
      p.materials.forEach(
        x => {
          let material = new Material();
          for (let key in x) {
            material[key] = x[key];
          }
          materialsArr.push(material);
        }
      );
      p.materials = materialsArr;
    }

    for (const key in p) {
      if (p.hasOwnProperty(key)) {
        plan[key] = p[key];
      }
    }
    return plan;
  }
}
