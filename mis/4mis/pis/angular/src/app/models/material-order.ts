import { NgbDateStruct } from '@ng-bootstrap/ng-bootstrap';

import { Material } from './material';

/** Material order */
export class MaterialOrder {
  private _orderId: number;
  public get orderId(): number {
    return this._orderId;
  }
  public set orderId(value: number) {
    this._orderId = value;
  }

  private _dateOfCreationObject: NgbDateStruct;
  public get dateOfCreationObject(): NgbDateStruct {
    return this._dateOfCreationObject;
  }
  public set dateOfCreationObject(value: NgbDateStruct) {
    this._dateOfCreationObject = value;
  }

  private _delivered: boolean = false;
  public get delivered(): boolean {
      return this._delivered;
  }
  public set delivered(value: boolean) {
      this._delivered = value;
  }

  private _materials: Array<Material>;
  public get materials(): Array<Material> {
    return this._materials;
  }
  public set materials(value: Array<Material>) {
    this._materials = value;
  }
  
  // copy obj values
  public copy(obj: MaterialOrder): void {
    this.orderId = obj.orderId;
    this.dateOfCreationObject = obj.dateOfCreationObject;
    this.delivered = obj.delivered;
    this.materials = [];
    // copy materials
    obj.materials.forEach(
      item => {
        const material = new Material();
        material.materialId = item.materialId;
        material.amount = item.amount;
        material.name = item.name;
        this.materials.push(material);
      }
    );
  }
  
  // clear object values
  public clear(): void {
    this._orderId = null;
    this._dateOfCreationObject = null;
    this._delivered = false;
    this._materials = [];
  }
  
  public get dateOfCreation(): string {
    if (this._dateOfCreationObject !== null &&
        this._dateOfCreationObject.hasOwnProperty('year') &&
        this._dateOfCreationObject.hasOwnProperty('month') &&
        this._dateOfCreationObject.hasOwnProperty('day')) {
      return String(this._dateOfCreationObject.year) + '-' +
             String(this._dateOfCreationObject.month).padStart(2, '0') + '-' +
             String(this._dateOfCreationObject.day).padStart(2, '0');
    } 
    else {
      return '';
    }
  }
  public set dateOfCreation(value: string) {
    const parts = value.split('-');
    this._dateOfCreationObject = {
      year:  Number(parts[0]),
      month: Number(parts[1]),
      day:   Number(parts[2])
    };
  }
  public get dateOfCreationFormattedCZ(): string {
    if (this._dateOfCreationObject !== null &&
        this._dateOfCreationObject.hasOwnProperty('year') &&
        this._dateOfCreationObject.hasOwnProperty('month') &&
        this._dateOfCreationObject.hasOwnProperty('day')) {
      return String(this._dateOfCreationObject['day']) + '.' +
             String(this._dateOfCreationObject['month']) + '.' +
             String(this._dateOfCreationObject['year']);
    } 
    else {
      return '';
    }
  }

  // getter for checking count of items
  public get gotMaterials(): boolean {
    if (!this._materials.length) {
      return false;
    }

    let resultFlag: boolean = true;
    // check if all materials have got specified amount and cost
    this._materials.forEach(
      m => {
        if (!m.amount || !m.cost) {
          resultFlag = false;
        }
      }
    );
    return resultFlag;
    // return this._materials.length > 0 && this._materials.filter(x => x.amount && x.cost).length > 0;
  }

  // order is ready to be posted to the server
  public readyForPost(): boolean {
    return this.gotMaterials;
  }

  // new order object for api
  public get apiNewOrder() {
    return {
      materials: this.materials.map( x => x.apiMaterialOrder )
    };
  }
}