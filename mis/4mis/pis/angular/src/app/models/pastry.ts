import { Material } from './material';
import { PastryCategory } from './pastry-category';

export class Pastry {
  constructor(obj = null) {
    if (obj) {
      Object.assign(this, obj);
    }
  }

  private _pastryId: number;
  public get pastryId(): number {
    return this._pastryId;
  }
  public set pastryId(value: number) {
    this._pastryId = value;
  }

  private _cost: number;
  public get cost(): number {
    return this._cost;
  }
  public set cost(value: number) {
    this._cost = value;
  }

  private _count: number = 0;
  public get count(): number {
    return this._count;
  }
  public set count(value: number) {
    this._gotAmount = true;
    this._count = value;
  }
  // aliases
  public get amount(): number {
    return this._count;
  }
  public set amount(value: number) {
    this._gotAmount = true;
    this._count = value;
  }

  private _name: string;
  public get name(): string {
    return this._name;
  }
  public set name(value: string) {
    this._name = value;
  }

  private _category: PastryCategory = new PastryCategory();
  public get category(): PastryCategory {
    return this._category;
  }
  public set category(value: PastryCategory) {
    this._category = value;
  }


  private _materials: Array<Material> = [];
  public get materials(): Array<Material> {
    return this._materials;
  }
  public set materials(value: Array<Material>) {
    this._materials = value;
  }

  private _quantity: number;
  public get quantity(): number {
    return this._quantity;
  }
  public set quantity(value: number) {
    this._quantity = value;
  }

  public get totalCost(): number {
    return this.cost * this.quantity;
  }

  private _storageChange: number = 0;
  public get storageChange(): number {
    return Number(this._storageChange); // TODO: why does it needs to be casted to Number?
  }
  public set storageChange(value: number) {
    this._storageChange = value;
  }

  private _gotAmount: boolean = false;
  public get gotAmount(): boolean {
    return this._gotAmount;
  }
  public set gotAmount(value: boolean) {
    this._gotAmount = value;
  }

  public get countLabel(): string {
    if (this.count === 1) {
      return 'kus';
    } else if (this.count > 1 && this.count < 5) {
      return 'kusy';
    } else {
      return 'kusů';
    }
  }

  public gotMaterial(): boolean {
    return this._materials.length > 0 && this.materials.filter(x => x.amount).length > 0;
  }

  // pastry is ready to be posted to the server
  public readyForPost(): boolean {
    return this.name !== '' && this.cost > 0 && this.gotMaterial();
  }

  // clear object values
  public clear(): void {
    this.name = '';
    this.cost = 0;
    this.materials = [];
  }

  // copy obj values
  public copy(obj: Pastry): void {
    this.pastryId = obj.pastryId;
    this.name = obj.name;
    this.cost = obj.cost;
    this.category.categoryId = obj.category.categoryId;
    this.materials = [];
    // copy materials
    obj.materials.forEach(item => {
      const material = new Material();
      material.materialId = item.materialId;
      material.amount = item.amount;
      material.name = obj.name;
      this.materials.push(material);
    });
  }

  private _allowEmitChange: boolean = false;
  public get allowEmitChange(): boolean {
    return this._allowEmitChange;
  }
  public set allowEmitChange(value: boolean) {
    this._allowEmitChange = value;
  }

  // New pastry
  public get apiNewPastry() {
    return {
      name: this.name,
      cost: this.cost,
      category: {
        categoryId: this.category.categoryId
      },
      materials: this.materials.map( x => x.apiAmount )
    };
  }

  // Update pastry
  public get apiUpdatePastry() {
    return {
      pastryId: this.pastryId,
      name: this.name,
      cost: this.cost,
      category: {
        categoryId: this.category.categoryId
      },
      materials: this.materials.map( x => x.apiAmount )
    };
  }

  // Pastry enter and withdraw
  public get apiAmountChange() {
    return {
      pastryId: this.pastryId,
      count: this.storageChange
    };
  }

  // Pastry as item of order
  public get apiPastryItem() {
    return {
      pastryId: this.pastryId,
      count: this.count
    };
  }
}
