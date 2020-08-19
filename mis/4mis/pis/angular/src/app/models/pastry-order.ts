import { NgbDateStruct } from '@ng-bootstrap/ng-bootstrap';

import { Pastry } from './pastry';
import { User } from './user';

/** Pastry order */
export class PastryOrder {
  private _orderId: number;
  public get orderId(): number {
    return this._orderId;
  }
  public set orderId(value: number) {
    this._orderId = value;
  }

  private _price: number;
  public get price(): number {
    return this._price;
  }
  public set price(value: number) {
    this._price = value;
  }

  private _state: string;
  public get state(): string {
    return this._state;
  }
  public set state(value: string) {
    this._state = value;
  }

  private _deliveryDateObject: NgbDateStruct;
  public get deliveryDateObject(): NgbDateStruct {
    return this._deliveryDateObject;
  }
  public set deliveryDateObject(value: NgbDateStruct) {
    this._deliveryDateObject = value;
  }

  private _customer: User;
  public get customer(): User {
    return this._customer;
  }
  public set customer(value: User) {
    this._customer = value;
  }

  private _items: Array<Pastry>;
  public get items(): Array<Pastry> {
    return this._items;
  }
  public set items(value: Array<Pastry>) {
    this._items = value;
  }

  // copy obj values
  public copy(obj: PastryOrder): void {
    this.orderId = obj.orderId;
    this.price = obj.price;
    this.state = obj.state;
    this.deliveryDateObject = obj.deliveryDateObject;
    if (obj.customer) {
      this.customer = obj.customer;
      this.customer.userId = obj.customer.userId;
      this.customer.name = obj.customer.name;
      this.customer.surname = obj.customer.surname;
      this.customer.email = obj.customer.email;
    }
    this.items = [];
    // copy items
    obj.items.forEach(
      i => {
        let item: Pastry = new Pastry();
        item.pastryId = i.pastryId;
        item.name = i.name;
        item.cost = i.cost;
        item.count = i.count;
        this.items.push(item);
      }
    );
  }

  // clear object values
  public clear(): void {
    this._orderId = null;
    this._customer = new User();
    // init tomorrow
    this._deliveryDateObject = this.minDate;
    this._items = [];
    this._price = 0;
    this._state = null;
  }

  // getter for tomorrow date
  public get tomorrow(): Date {
    let today: Date = new Date();
    let tomorrow: Date = new Date(today);
    tomorrow.setDate(tomorrow.getDate() + 1);
    tomorrow.setHours(0,0,0,0);

    return tomorrow;
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
  // maximum next year?
  public get maxDate(): NgbDateStruct {
    let today: Date = new Date();
    return {
      year:  today.getFullYear() + 1,
      month: today.getMonth() + 1,
      day:   today.getDate()
    };
  }

  public get isTomorrowOrLater(): boolean {
    return (
      this._deliveryDateObject != null &&
      this._deliveryDateObject.hasOwnProperty('year') &&
      this._deliveryDateObject.hasOwnProperty('month') &&
      this._deliveryDateObject.hasOwnProperty('day') &&
      new Date(this._deliveryDateObject.year, this._deliveryDateObject.month - 1, this._deliveryDateObject.day) >= this.tomorrow
    );
  }

  public get deliveryDate(): string {
    if (this._deliveryDateObject !== null &&
        this._deliveryDateObject.hasOwnProperty('year') &&
        this._deliveryDateObject.hasOwnProperty('month') &&
        this._deliveryDateObject.hasOwnProperty('day')) {
      return String(this._deliveryDateObject.year) + '-' +
             String(this._deliveryDateObject.month).padStart(2, '0') + '-' +
             String(this._deliveryDateObject.day).padStart(2, '0');
    } 
    else {
      return '';
    }
  }
  public set deliveryDate(value: string) {
    const parts = value.split('-');
    this._deliveryDateObject = {
      year:  Number(parts[0]),
      month: Number(parts[1]),
      day:   Number(parts[2])
    };
  }
  public get deliveryDateFormattedCZ(): string {
    if (this._deliveryDateObject !== null &&
        this._deliveryDateObject.hasOwnProperty('year') &&
        this._deliveryDateObject.hasOwnProperty('month') &&
        this._deliveryDateObject.hasOwnProperty('day')) {
      return String(this._deliveryDateObject['day']) + '.' +
             String(this._deliveryDateObject['month']) + '.' +
             String(this._deliveryDateObject['year']);
    } 
    else {
      return '';
    }
  }

  // getter for checking count of items
  public get gotItems(): boolean {
    return this._items.length > 0 && this._items.filter(x => x.count).length > 0;
  }

  // order is ready to be posted to the server
  public readyForPost(): boolean {
    return (
      this._state !== '' && 
      this._price > 0 && 
      this.isTomorrowOrLater && 
      this._customer !== null && this._customer !== undefined && this._customer.userId !== null && 
      this.gotItems
    );
  }

  // order is ready to be posted to the server
  public readyForPut(): boolean {
    return (
      this._orderId &&
      this._state !== '' && 
      this._price > 0 && 
      this._customer !== null && this._customer !== undefined && this._customer.userId !== null && 
      this.gotItems
    );
  }

  // new order object for api
  public get apiNewOrder() {
    return {
      price: this.price,
      state: this.state,
      deliveryDate: this.deliveryDate,
      customer: {
        'userId': this.customer.userId
      },
      items: this.items.map( x => x.apiPastryItem )
    };
  }

  // new order object for api
  public get apiEditOrder() {
    return {
      orderId: this.orderId,
      price: this.price,
      state: this.state,
      deliveryDate: this.deliveryDate,
      customer: {
        'userId': this.customer.userId
      },
      items: this.items.map( x => x.apiPastryItem )
    };
  }
}