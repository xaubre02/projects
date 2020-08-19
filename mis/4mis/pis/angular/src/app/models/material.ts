export class Material {

  constructor(name?: string) {
    if (name !== undefined) {
      this._name = name;
    }
  }

  private _materialId: number;
  public get materialId(): number {
    return Number(this._materialId);
  }
  public set materialId(value: number) {
    this._materialId = value;
  }

  private _amount: number = 0;
  public get amount(): number {
    return this._amount;
  }
  public set amount(value: number) {
    this._gotAmount = true;
    this._amount = value;
  }

  private _cost: number = 0;
  public get cost(): number {
    return this._cost;
  }
  public set cost(value: number) {
    this._cost = value;
  }

  private _name: string;
  public get name(): string {
    return this._name;
  }
  public set name(value: string) {
    this._name = value;
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

  private _isEnough: boolean;
  public get isEnough(): boolean {
    return this._isEnough;
  }
  public set isEnough(value: boolean) {
    this._isEnough = value;
  }

  private _diff: number;
  public get diff(): number {
    return this._diff;
  }
  public set diff(value: number) {
    this._diff = value;
  }

  // New material
  public get apiNewMaterial() {
    return {
      name: this.name
    };
  }

  // New material
  public get apiUpdateMaterial() {
    return {
      materialId: this.materialId,
      name: this.name
    };
  }

  // Material amount
  public get apiAmount() {
    return {
      materialId: this.materialId,
      amount: this.amount
    };
  }

  // Material enter and withdraw
  public get apiAmountChange() {
    return {
      materialId: this.materialId,
      amount: this.storageChange
    };
  }

  // Material order api format
  public get apiMaterialOrder() {
    return {
      materialId: this.materialId,
      amount: this.amount,
      cost: this.cost
    };
  }
}
