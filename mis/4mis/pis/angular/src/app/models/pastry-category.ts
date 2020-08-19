export class PastryCategory {

  constructor(name?: string) {
    if (name !== undefined) {
      this._name = name;
    }
  }

  private _categoryId: number;
  public get categoryId(): number {
    return Number(this._categoryId);
  }
  public set categoryId(value: number) {
    this._categoryId = value;
  }

  private _name: string;
  public get name(): string {
    return this._name;
  }
  public set name(value: string) {
    this._name = value;
  }

  // New category
  public get apiNewCategory() {
    return {
      name: this.name
    };
  }

  // Update category
  public get apiUpdateCategory() {
    return {
      categoryId: this.categoryId,
      name: this.name
    };
  }
}