import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Observable, BehaviorSubject } from 'rxjs';

import { Material } from '../models';
import { serviceConfiguration } from '../config';
import { AuthenticationService } from 'src/app/services/authentication.service';
import { AlertService } from './alert.service';

@Injectable({
  providedIn: 'root'
})
export class MaterialService {

  private _materials: BehaviorSubject<Array<Material>> = new BehaviorSubject([]);
  private _materialsCache: Array<Material> = [];
  private _loadingMaterial: boolean = false;
  private _loadingMaterialAmount: boolean = false;

  constructor(
    private _http: HttpClient,
    private _authService: AuthenticationService,
    private _alertService: AlertService
  ) { }

  public get loadingMaterial(): boolean {
    return this._loadingMaterial;
  }

  public get loadingAmount(): boolean {
    return this._loadingMaterialAmount;
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

  public postMaterial(material: Material): void {
    this._http.post<any>(serviceConfiguration.materials.apiPost, material.apiNewMaterial, this.authHeader).subscribe(
      _ => {
        this.getMaterials();
        this._alertService.success('Nová surovina \'' + material.name + '\' byla úspěšně přidána.');
      },
      error => {
        if (error.statusText === 'Conflict') {
          this._alertService.error('Surovina se zadaným názvem již existuje!');
        } else {
          this._alertService.error(error.statusText);
        }
      }
    );
  }

  public updateMaterial(material: Material): void {
    this._http.put<any>(serviceConfiguration.materials.apiPut + material.materialId, material.apiUpdateMaterial, this.authHeader).subscribe(
      _ => {
        this.getMaterials();
        this._alertService.success('Změna proběhla v pořádku.');
      },
      error => {
        if (error.statusText === 'Conflict') {
          this._alertService.error('Surovinu se nepodařilo upravit!');
        } else {
          this._alertService.error(error.statusText);
        }
      }
    );
  }

  public deleteMaterial(material: Material): void {
    this._http.delete(serviceConfiguration.materials.apiDelete + material.materialId, this.authHeader).subscribe(
      _ => {
        this.getMaterials();
        this._alertService.success('Odstranění proběhlo v pořádku.');
      },
      error => {
        this._alertService.error('Surovinu se nepodařilo odstranit: ' + error.error.error);
      }
    );
  }

  public enterAmount(material: Material): void {
    material.gotAmount = false;
    this._http.post<any>(serviceConfiguration.materials.apiEnter, material.apiAmountChange, this.authHeader).subscribe(
      _ => {
        this.getMaterialsAmount();
        this._alertService.success('Naskladnění proběhlo v pořádku.');
      },
      error => {
        this._alertService.error('Surovinu \'' + material.name + '\' se nepodařilo naskladnit: ' + error.statusText);
        material.gotAmount = true;
      },
      () => {
        material.storageChange = 0;
      }
    );
  }

  public enterAmountList(materials: Array<Material>): void {
    materials.forEach(material => material.gotAmount = false);
    this._http.post<any>(serviceConfiguration.materials.apiEnterList, materials.map(x => x.apiAmountChange), this.authHeader).subscribe(
      _ => {
        this.getMaterialsAmount();
        this._alertService.success('Naskladnění proběhlo v pořádku.');
      },
      error => {
        this._alertService.error('Suroviny se nepodařilo naskladnit: ' + error.statusText);
        materials.forEach(material => material.gotAmount = true);
      },
      () => {
        materials.forEach(material => material.storageChange = 0);
      }
    );
  }

  public withdrawAmount(material: Material): void {
    material.gotAmount = false;
    this._http.post<any>(serviceConfiguration.materials.apiWithdraw, material.apiAmountChange, this.authHeader).subscribe(
      _ => {
        this.getMaterialsAmount();
        this._alertService.success('Vyskladnění proběhlo v pořádku.');
      },
      error => {
        this._alertService.error('Surovinu \'' + material.name + '\' se nepodařilo vyskladnit: ' + error.statusText);
        material.gotAmount = true;
      },
      () => {
        material.storageChange = 0;
      }
    );
  }

  public withdrawAmountList(materials: Array<Material>): void {
    materials.forEach(material => material.gotAmount = false);
    this._http.post<any>(serviceConfiguration.materials.apiWithdrawList, materials.map(x => x.apiAmountChange), this.authHeader).subscribe(
      _ => {
        this.getMaterialsAmount();
        this._alertService.success('Vyskladnění proběhlo v pořádku.');
      },
      error => {
        this._alertService.error('Suroviny se nepodařilo vyskladnit: ' + error.statusText);
        materials.forEach(material => material.gotAmount = true);
      },
      () => {
        materials.forEach(material => material.storageChange = 0);
      }
    );
  }

  public getMaterials(): Observable<Array<Material>> {
    this._loadingMaterial = true;
    this._http.get(serviceConfiguration.materials.apiList, this.authHeader).subscribe(
      (response: Array<any>) => {
        this._materialsCache = this.buildMaterialsFromData(response);
        this._materialsCache.sort((a, b) => (a.name > b.name) ? 1 : -1);
        this._materials.next(this._materialsCache);
        this._loadingMaterial = false;
        // get material amount
        this.getMaterialsAmount();
      },
      error => {
        this._alertService.error('Nepodařilo se načíst suroviny: ' + error.statusText);
        this._loadingMaterial = false;
      }
    );

    return this._materials.asObservable();
  }

  public getMaterialsAmount(): void {
    if (this.userIsAuthorized) {
      this._loadingMaterialAmount = true;
      this._http.get(serviceConfiguration.materials.apiAmount, this.authHeader).subscribe(
        (response: Array<any>) => {
          this.updateMaterialAmount(response);
          this._loadingMaterialAmount = false;
        },
        error => {
          this._alertService.error('Nepodařilo se načíst množství surovin: ' + error.statusText);
          this._loadingMaterialAmount = false;
        }
      );
    }
  }

  private updateMaterialAmount(materials: Array<any>): void {
    materials.forEach(
      item => {
        if (item.hasOwnProperty('amount') && item.hasOwnProperty('material')) {
          const material: Material = this.buildMaterial(item.material);
          // update value
          const current = this._materialsCache.find(m => m.materialId === material.materialId);
          if (current && current.amount !== item.amount) {
            current.amount = item.amount;
          }
        }
      }
    );
    this._materialsCache.forEach(
      item => {
        item.gotAmount = true;
    });
  }

  private buildMaterialsFromData(data: Array<any>): any {
    if (!data) { return []; }
    const materials: Array<Material> = [];
    data.forEach(
      m => {
        const material = this.buildMaterial(m);
        // already got material
        const current = this._materialsCache.find(mat => mat.materialId === material.materialId);
        if (current && current.gotAmount) {
          material.amount = current.amount;
        }
        materials.push(material);
      }
    );
    return materials;
  }

  private buildMaterial(m: any): Material {
    const material: Material = new Material();
    for (const key in m) {
      if (m.hasOwnProperty(key)) {
        material[key] = m[key];
      }
    }
    return material;
  }
}
