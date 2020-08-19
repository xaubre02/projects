import { NgbDateStruct } from '@ng-bootstrap/ng-bootstrap';

import { Pastry } from './pastry';
import { Material } from './material';

/** Production plan */
export class ProductionPlan {
  private _planId: number;
  public get planId(): number {
    return this._planId;
  }
  public set planId(value: number) {
    this._planId = value;
  }

  private _accepted: boolean = false;
  public get accepted(): boolean {
    return this._accepted;
  }
  public set accepted(value: boolean) {
    this._accepted = value;
  }

  private _productionDateObject: NgbDateStruct;
  public get productionDateObject(): NgbDateStruct {
    return this._productionDateObject;
  }
  public set productionDateObject(value: NgbDateStruct) {
    this._productionDateObject = value;
  }

  private _items: Array<Pastry>;
  public get items(): Array<Pastry> {
    return this._items;
  }
  public set items(value: Array<Pastry>) {
    this._items = value;
  }
  
  private _materials: Array<Material>;
  public get materials(): Array<Material> {
    return this._materials;
  }
  public set materials(value: Array<Material>) {
    this._materials = value;
  }

  // copy obj values
  public copy(obj: ProductionPlan): void {
    this.planId = obj.planId;
    this.accepted = obj.accepted;
    this.productionDateObject = obj.productionDateObject;
    this.items = [];
    this.materials = [];
    // copy items
    obj.items.forEach(
      i => {
        let item: Pastry = new Pastry();
        item.pastryId = i.pastryId;
        item.name = i.name;
        item.count = i.count;
        this.items.push(item);
      }
    );
    // copy materials
    obj.materials.forEach(
      m => {
        let material: Material = new Material();
        material.materialId = m.materialId;
        material.name = m.name;
        material.amount = m.amount;
        material.isEnough = m.isEnough;
        material.diff = m.diff;
        this.materials.push(material);
      }
    );
  }

  // clear object values
  public clear(): void {
    this._planId = null;
    // this._accepted = null;
    this._productionDateObject = null;
    this._items = [];
  }

  // minimum tomorrow
  public get minDate(): NgbDateStruct {
    let tomorrow: Date = this.tomorrow;
    return {
      year:  tomorrow.getFullYear(),
      month: tomorrow.getMonth() + 1,
      day:   tomorrow.getDate()
    };
  }
  // minimum today
  public get minDateToday(): NgbDateStruct {
    let today: Date = new Date();
    return {
      year:  today.getFullYear(),
      month: today.getMonth() + 1,
      day:   today.getDate()
    };
  }
  // maximum next year?
  public get maxDate(): NgbDateStruct {
    let today: Date = new Date();
    return {
      year:  today.getFullYear() + 1,
      month: today.getMonth() + 1,
      day:   today.getDate()
    };
  }

  // getter for tomorrow date
  public get tomorrow(): Date {
    let today: Date = new Date();
    let tomorrow: Date = new Date(today);
    tomorrow.setDate(tomorrow.getDate() + 1);
    tomorrow.setHours(0,0,0,0);

    return tomorrow;
  }
  public get isTomorrowOrLater(): boolean {
    return (
      this._productionDateObject != null &&
      this._productionDateObject.hasOwnProperty('year') &&
      this._productionDateObject.hasOwnProperty('month') &&
      this._productionDateObject.hasOwnProperty('day') &&
      new Date(this._productionDateObject.year, this._productionDateObject.month - 1, this._productionDateObject.day) >= this.tomorrow
    );
  }
  public get isTodayOrLater(): boolean {
    let today: Date = new Date();
    today.setHours(0,0,0,0);
    return (
      this._productionDateObject != null &&
      this._productionDateObject.hasOwnProperty('year') &&
      this._productionDateObject.hasOwnProperty('month') &&
      this._productionDateObject.hasOwnProperty('day') &&
      new Date(this._productionDateObject.year, this._productionDateObject.month - 1, this._productionDateObject.day) >= today
    );
  }

  public get productionDateAsDate(): Date {
    if (this._productionDateObject !== null &&
        this._productionDateObject.hasOwnProperty('year') &&
        this._productionDateObject.hasOwnProperty('month') &&
        this._productionDateObject.hasOwnProperty('day')) {
      return new Date(this._productionDateObject.year, this._productionDateObject.month - 1, this._productionDateObject.day);
    } 
    else {
      return null;
    }
  }
  public get productionDate(): string {
    if (this._productionDateObject !== null &&
        this._productionDateObject.hasOwnProperty('year') &&
        this._productionDateObject.hasOwnProperty('month') &&
        this._productionDateObject.hasOwnProperty('day')) {
      return String(this._productionDateObject.year) + '-' +
             String(this._productionDateObject.month).padStart(2, '0') + '-' +
             String(this._productionDateObject.day).padStart(2, '0');
    } 
    else {
      return '';
    }
  }
  public set productionDate(value: string) {
    const parts = value.split('-');
    this._productionDateObject = {
      year:  Number(parts[0]),
      month: Number(parts[1]),
      day:   Number(parts[2])
    };
  }
  public get productionDateFormattedCZ(): string {
    if (this._productionDateObject !== null &&
        this._productionDateObject.hasOwnProperty('year') &&
        this._productionDateObject.hasOwnProperty('month') &&
        this._productionDateObject.hasOwnProperty('day')) {
      return String(this._productionDateObject['day']) + '.' +
             String(this._productionDateObject['month']) + '.' +
             String(this._productionDateObject['year']);
    } 
    else {
      return '';
    }
  }

  
  // getter for checking count of items
  public get gotItems(): boolean {
    return this._items.length > 0 && this._items.filter(x => x.count).length > 0;
  }

  // getter for checking if plan has enough materials for production
  public get notEnoughMaterials(): boolean {
    return this.isTodayOrLater && this._materials.length > 0 && this._materials.filter(x => !x.isEnough && x.diff).length > 0;
  }

  // plan object is ready to get proposal
  public get readyForProposal(): boolean {
    return this.isTomorrowOrLater;
  }

  // plan object is ready to post request
  public get readyForPost(): boolean {
    return (
      this.isTomorrowOrLater &&
      this.gotItems
    );
  }

  // plan object is ready to put request
  public get readyForPut(): boolean {
    return (
      this._planId &&
      this.isTodayOrLater &&
      this.gotItems
    );
  }

  // new plan object for api
  public get apiNewPlan() {
    return {
      accepted: true,
      productionDate: this.productionDate,
      items: this.items.map( x => x.apiPastryItem )
    };
  }

  // edit plan object for api
  public get apiEditPlan() {
    return {
      planId: this.planId,
      accepted: true,
      productionDate: this.productionDate,
      items: this.items.map( x => x.apiPastryItem )
    };
  }
}