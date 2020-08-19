import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Observable, BehaviorSubject } from 'rxjs';

import { Pastry, Material, PastryCategory } from '../models';
import { serviceConfiguration } from '../config';
import { AuthenticationService } from 'src/app/services/authentication.service';
import { AlertService } from './alert.service';

@Injectable({
  providedIn: 'root'
})
export class PastryService {

  private _pastries: BehaviorSubject<Array<Pastry>> = new BehaviorSubject([]);
  private _pastriesCache: Array<Pastry> = [];
  private _categories: BehaviorSubject<Array<PastryCategory>> = new BehaviorSubject([]);
  private _categoriesCache: Array<PastryCategory> = [];
  private _loadingPastry: boolean = false;
  private _loadingPastryAmount: boolean = false;
  private _loadingCategories: boolean = false;

  constructor(
    private _http: HttpClient,
    private _authService: AuthenticationService,
    private _alertService: AlertService
  ) { }

  public get loadingPastry(): boolean {
    return this._loadingPastry;
  }

  public get loadingAmount(): boolean {
    return this._loadingPastryAmount;
  }

  public get loadingCategories(): boolean {
    return this._loadingCategories;
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

  public postPastry(pastry: Pastry): void {
    let postPastry: any = pastry.apiNewPastry;
    this._http.post<any>(serviceConfiguration.pastry.apiPostPastry, postPastry, this.authHeader).subscribe(
      _ => {
        this.getPastries();
        this._alertService.success('Nové pečivo \'' + postPastry.name + '\' bylo úspěšně přidáno.');
      },
      error => {
        console.log(error);
        if (error.statusText === 'Conflict') {
          this._alertService.error('Pečivo se zadaným názvem již existuje!');
        } else {
          this._alertService.error(error.statusText);
        }
      }
    );
  }

  public updatePastry(pastry: Pastry): void {
    this._http.put<any>(serviceConfiguration.pastry.apiPutPastry + pastry.pastryId, pastry.apiUpdatePastry, this.authHeader).subscribe(
      _ => {
        this.getPastries();
        this._alertService.success('Změna proběhla v pořádku.');
      },
      error => {
        if (error.statusText === 'Conflict') {
          this._alertService.error('Pečivo se nepodařilo upravit!');
        } else {
          this._alertService.error(error.statusText);
        }
      }
    );
  }

  public deletePastry(pastry: Pastry): void {
    this._http.delete(serviceConfiguration.pastry.apiDeletePastry + pastry.pastryId, this.authHeader).subscribe(
      _ => {
        this.getPastries();
        this._alertService.success('Odstranění proběhlo v pořádku.');
      },
      error => {
        this._alertService.error('Pečivo se nepodařilo odstranit: ' + error.error.error);
      }
    );
  }

  public postCategory(category: PastryCategory): void {
    this._http.post<any>(serviceConfiguration.pastry.apiPostCategory, category.apiNewCategory, this.authHeader).subscribe(
      _ => {
        this.getCategories();
        this._alertService.success('Nová kategorie \'' + category.name + '\' byla úspěšně přidána.');
      },
      error => {
        if (error.statusText === 'Conflict') {
          this._alertService.error('Kategorie se zadaným názvem již existuje!');
        } else {
          this._alertService.error(error.statusText);
        }
      }
    );
  }

  public updateCategory(category: PastryCategory): void {
    this._http.put<any>(serviceConfiguration.pastry.apiPutCategory + category.categoryId, category.apiUpdateCategory, this.authHeader).subscribe(
      _ => {
        this.getCategories();
        this._alertService.success('Změna proběhla v pořádku.');
      },
      error => {
        if (error.statusText === 'Conflict') {
          this._alertService.error('Kategorii se nepodařilo upravit!');
        } else {
          this._alertService.error(error.statusText);
        }
      }
    );
  }

  public deleteCategory(category: PastryCategory): void {
    this._http.delete(serviceConfiguration.pastry.apiDeleteCategory + category.categoryId, this.authHeader).subscribe(
      _ => {
        this.getCategories();
        this._alertService.success('Odstranění proběhlo v pořádku.');
      },
      error => {
        this._alertService.error('Kategorii se nepodařilo odstranit: ' + error.error.error);
      }
    );
  }

  public enterAmount(pastry: Pastry): void {
    pastry.gotAmount = false;
    this._http.post<any>(serviceConfiguration.pastry.apiEnter, pastry.apiAmountChange, this.authHeader).subscribe(
      _ => {
        this.getPastriesAmount();
        this._alertService.success('Naskladnění proběhlo v pořádku.');
      },
      error => {
        this._alertService.error('Pečivo \'' + pastry.name + '\' se nepodařilo naskladnit: ' + error.statusText);
        pastry.gotAmount = true;
      },
      () => {
        pastry.storageChange = 0;
      }
    );
  }

  public enterAmountList(pastries: Array<Pastry>): void {
    pastries.forEach(pastry => pastry.gotAmount = false);
    this._http.post<any>(serviceConfiguration.pastry.apiEnterList, pastries.map(x => x.apiAmountChange), this.authHeader).subscribe(
      _ => {
        this.getPastriesAmount();
        this._alertService.success('Naskladnění proběhlo v pořádku.');
      },
      error => {
        this._alertService.error('Naskladnění pečiva se nezdařilo: ' + error.statusText);
        pastries.forEach(pastry => pastry.gotAmount = true);
      },
      () => {
        pastries.forEach(pastry => pastry.storageChange = 0);
      }
    );
  }

  public withdrawAmount(pastry: Pastry): void {
    pastry.gotAmount = false;
    this._http.post<any>(serviceConfiguration.pastry.apiWithdraw, pastry.apiAmountChange, this.authHeader).subscribe(
      _ => {
        this.getPastriesAmount();
        this._alertService.success('Vyskladnění proběhlo v pořádku.');
      },
      error => {
        this._alertService.error('Pečivo \'' + pastry.name + '\' se nepodařilo vyskladnit: ' + error.statusText);
        pastry.gotAmount = true;
      },
      () => {
        pastry.storageChange = 0;
      }
    );
  }

  public withdrawAmountList(pastries: Array<Pastry>): void {
    pastries.forEach(pastry => pastry.gotAmount = false);
    this._http.post<any>(serviceConfiguration.pastry.apiWithdrawList, pastries.map(x => x.apiAmountChange), this.authHeader).subscribe(
      _ => {
        this.getPastriesAmount();
        this._alertService.success('Vyskladnění proběhlo v pořádku.');
      },
      error => {
        this._alertService.error('Vyskladnění pečiva se nezdařilo: ' + error.statusText);
        pastries.forEach(pastry => pastry.gotAmount = true);
      },
      () => {
        pastries.forEach(pastry => pastry.storageChange = 0);
      }
    );
  }

  public getPastries(getAmount: boolean = true): Observable<Array<Pastry>> {
    this._loadingPastry = true;
    this._http.get(serviceConfiguration.pastry.apiList).subscribe(
      (response: Array<any>) => {
        this._pastriesCache = this.buildPastriesFromData(response);
        this._pastriesCache.sort((a, b) => (a.name > b.name) ? 1 : -1);
        this._pastries.next(this._pastriesCache);
        this._loadingPastry = false;
        // get pastry amount
        if (getAmount) {
          this.getPastriesAmount();
        }
      },
      error => {
        this._alertService.error('Nepodařilo se načíst pečivo: ' + error.statusText);
        this._loadingPastry = false;
      }
    );

    return this._pastries.asObservable();
  }

  public getCategories(): Observable<Array<PastryCategory>> {
    this._loadingCategories = true;
    this._http.get(serviceConfiguration.pastry.apiCategoryList).subscribe(
      (response: Array<any>) => {
        this._categoriesCache = this.buildPastryCategoryFromData(response);
        this._categories.next(this._categoriesCache);
        this._loadingCategories = false;
      },
      error => {
        this._alertService.error('Nepodařilo se načíst kategorie pečiva: ' + error.statusText);
        this._loadingCategories = false;
      }
    );

    return this._categories.asObservable();
  }

  public getPastriesAmount(): void {
    if (this.userIsAuthorized) {
      this._loadingPastryAmount = true;
      this._http.get(serviceConfiguration.pastry.apiAmount, this.authHeader).subscribe(
        (response: Array<any>) => {
          this.updatePastryAmount(response);
          this._loadingPastryAmount = false;
        },
        error => {
          this._alertService.error('Nepodařilo se načíst množství pečiva: ' + error.statusText);
          this._loadingPastryAmount = false;
        }
      );
    }
  }

  private updatePastryAmount(pastries: Array<any>): void {
    pastries.forEach(
      item => {
        if (item.hasOwnProperty('count') && item.hasOwnProperty('pastry')) {
          const pastry: Pastry = this.buildPastry(item.pastry);
          // update value
          const current = this._pastriesCache.find(p => p.pastryId === pastry.pastryId);
          if (current && current.count !== item.count) {
            current.count = item.count;
          }
        }
      }
    );
    this._pastriesCache.forEach(
      item => {
        item.gotAmount = true;
    });
  }

  private buildPastriesFromData(data: Array<any>): any {
    if (!data) { return []; }

    const pastries: Array<Pastry> = [];
    data.forEach(
      p => {
        const pastry = this.buildPastry(p);
        pastries.push(pastry);
      }
    );
    return pastries;
  }

  private buildPastry(p: any): Pastry {
    const pastry: Pastry = new Pastry();

    // solve nested json objects
    if (p.materials && p.materials.length) {
      const materialsArr = new Array<Material>();
      p.materials.forEach(
        m => {
          const material = new Material();
          for (const key in m) {
            if (m.hasOwnProperty(key)) {
              material[key] = m[key];
            }
          }
          materialsArr.push(material);
        }
      );
      p.materials = materialsArr;
    }

    for (const key in p) {
      if (p.hasOwnProperty(key)) {
        pastry[key] = p[key];
      }
    }

    return pastry;
  }

  private buildPastryCategoryFromData(data: Array<any>): any {
    if (!data) { return []; }

    const categories: Array<PastryCategory> = [];
    data.forEach(
      cat => {
        const category = this.buildCategory(cat);
        categories.push(category);
      }
    );
    return categories;
  }

  private buildCategory(m: any): PastryCategory {
    const category: PastryCategory = new PastryCategory();
    for (const key in m) {
      if (m.hasOwnProperty(key)) {
        category[key] = m[key];
      }
    }
    return category;
  }
}
