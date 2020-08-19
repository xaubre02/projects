import { Component, OnInit, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';
import { NgbModal } from '@ng-bootstrap/ng-bootstrap';

import { PastryService, MaterialService, AlertService, AuthenticationService } from '../../services';
import { Pastry, PastryCategory, Material } from '../../models';
import { userRolesDb } from '../../config';

@Component({
  selector: 'app-expedition',
  templateUrl: './expedition.component.html',
  styleUrls: ['./expedition.component.css']
})
export class ExpeditionComponent implements OnInit, OnDestroy {
  private _subscribed: Array<Subscription> = [];
  private _itemList: Array<Pastry>;
  private _categoryList: Array<PastryCategory>;
  private _materialList: Array<Material>;
  private _submitted: boolean = false;
  private _newCategoryName: string = '';
  private _clickedIndex: number;
  private _dialogConfig = {
    type: '',
    title: '',
    buttonText: '',
    buttonClass: ''
  }
  private _selectedCategory: number = -1;
  private _newPastry: Pastry = new Pastry();

  public reset: any[] = [{}];

  constructor(
    private authService: AuthenticationService,
    private itemsService: PastryService,
    private materialService: MaterialService,
    private modalService: NgbModal,
    private alertService: AlertService
  ) {
    this.newPastry.clear();
  }

  ngOnInit(): void {
    this.loadItems();
  }

  ngOnDestroy() {
    this._subscribed.forEach(
      subsriber => {
        subsriber.unsubscribe();
      }
    );
  }

  public get itemList(): Array<Pastry> {
    if (this.selectedCategory === -1) {
      return this._itemList;
    } else {
      return this._itemList.filter(x => x.category.categoryId === this.selectedCategory);
    }
  }
  public get categoryList(): Array<PastryCategory> { return this._categoryList; }
  public get materialList(): Array<Material>       { return this._materialList; }
  public get submitted(): boolean                  { return this._submitted; }
  public get newCategoryName(): string             { return this._newCategoryName; }
  public set newCategoryName(value: string)        { this._newCategoryName = value; }
  public get dialogConfig()                        { return this._dialogConfig; }
  public get loadingItems(): boolean               { return this.itemsService.loadingPastry; }
  public get loadingAmount(): boolean              { return this.itemsService.loadingAmount; }
  public get loadingCategories(): boolean          { return this.itemsService.loadingCategories; }
  public get clickedItem(): Pastry                 { return this._itemList[this._clickedIndex]; }
  public get clickedCategory(): PastryCategory     { return this._categoryList[this._clickedIndex]; }
  public get selectedCategory(): number            { return Number(this._selectedCategory); }
  public set selectedCategory(value: number)       { this._selectedCategory = value; }
  public get newPastry(): Pastry                   { return this._newPastry; }
  public get availableMaterials(): Array<Material> {
    return this._materialList.filter(
      x => !(this.newPastry.materials.find(m => m.materialId === x.materialId))
    );
  }

  public get userIsAuthorized(): boolean {
    return this.authService.loggedUser !== null && this.authService.loggedUser.role !== 'customer';
  }
  public get userManager(): boolean {
    if (this.authService.loggedUser) {
      return this.authService.loggedUser.role == userRolesDb.manager;
    }
    return false;
  }
  public get userBaker(): boolean {
    if (this.authService.loggedUser) {
      return this.authService.loggedUser.role == userRolesDb.baker;
    }
    return false;
  }
  public get userPastryStoreman(): boolean {
    if (this.authService.loggedUser) {
      return this.authService.loggedUser.role == userRolesDb.pastrystoreman;
    }
    return false;
  }
  public get userMaterialStoreman(): boolean {
    if (this.authService.loggedUser) {
      return this.authService.loggedUser.role == userRolesDb.materialstoreman;
    }
    return false;
  }

  private loadItems(): void {
    this._subscribed.forEach(
      subscriber => {
        subscriber.unsubscribe();
      }
    );
    this._subscribed = [];

    // load pastry categories
    this._subscribed.push(
      this.itemsService.getCategories().subscribe(
        response => {
          if (response && this._categoryList !== response && response.length) {
            this._categoryList = response;
          } else if (this._categoryList !== response) {
            this._categoryList = [];
          }
        }
    ));

    // load pastries itself
    this._subscribed.push(
      this.itemsService.getPastries().subscribe(
        response => {
          if (response && this._itemList !== response && response.length) {
            this._itemList = response;
          } else if (this._itemList !== response) {
            this._itemList = [];
          }
        }
    ));

    // load materials
    this._subscribed.push(
      this.materialService.getMaterials().subscribe(
        response => {
          if (response && this._materialList !== response && response.length) {
            this._materialList = response;
          } else if (this._materialList !== response) {
            this._materialList = [];
          }
        }
    ));
  }

  public trackByPastry(index: number, item: Pastry) {
    return item ? item.pastryId : null;
  }

  public trackByPastryCategory(index: number, item: PastryCategory) {
    return item ? item.categoryId : null;
  }

  public newPastryAddMaterial(): void {
    const material = new Material();
    if (this._materialList.length) {
      material.materialId = -1;
    }
    this._newPastry.materials.push(material);
  }

  public newPastryRemoveMaterial(index: number): void {
    this._newPastry.materials.splice(index, 1);
  }

  public getMaterialNameById(id: number): string {
    const material = this.materialList.find(x => x.materialId === id);
    return material ? material.name : '';
  }

  public openDialog(dialog: any, type: string, index?: number): void {
    this._clickedIndex = index;
    switch (type) {
      // new pastry dialog
      case 'newPastry':
        // default selected category
        if (this._categoryList.length) {
          this.newPastry.category.categoryId = this._categoryList[0].categoryId;
        } else {
          this.alertService.error('Nejsou k dispozici žádné kategorie! Pro přidání pečiva nejdříve vytvořte novou kategorii.');
          return;
        }
        this._dialogConfig = {
          type: 'newPastry',
          title: 'Přidat nové pečivo',
          buttonText: 'Přidat',
          buttonClass: 'btn-success'
        }
        break;
      // edit pastry dialog
      case 'editPastry':
        // default selected category
        this.newPastry.copy(this.clickedItem);
        this._dialogConfig = {
          type: 'editPastry',
          title: 'Upravit pečivo \'' + this.newPastry.name + '\'',
          buttonText: 'Upravit',
          buttonClass: 'btn-primary'
        }
        break;
      // new category dialog
      case 'newCategory':
        this._dialogConfig = {
          type: 'newCategory',
          title: 'Přidat novou kategorii',
          buttonText: 'Přidat',
          buttonClass: 'btn-success'
        }
        break;
      // edit category dialog
      case 'editCategory':
        this._newCategoryName = this.clickedCategory.name;
        this._dialogConfig = {
          type: 'editCategory',
          title: 'Upravit kategorii \'' + this._newCategoryName + '\'',
          buttonText: 'Upravit',
          buttonClass: 'btn-primary'
        }
        break;
      // enter amount dialog
      case 'enter':
        this._dialogConfig = {
          type: 'enter',
          title: 'Naskladnit: ' + this.clickedItem.name,
          buttonText: 'Naskladnit',
          buttonClass: 'btn-success'
        }
        break;
      // withdraw amount dialog
      case 'withdraw':
        this._dialogConfig = {
          type: 'withdraw',
          title: 'Vyskladnit: ' + this.clickedItem.name,
          buttonText: 'Vyskladnit',
          buttonClass: 'btn-danger'
        }
        break;
      // delete pastry dialog
      case 'deletePastry':
        this._dialogConfig = {
          type: 'deletePastry',
          title: 'pečivo',
          buttonText: 'toto pečivo',
          buttonClass: 'btn-danger'
        }
        break;
      // delete pastry category
      default:
        this._dialogConfig = {
          type: 'deletePastryCategory',
          title: 'kategorii',
          buttonText: 'tuto kategorii',
          buttonClass: 'btn-danger'
        }
    }
    this.modalService.open(dialog, {windowClass: 'modalWindow'});
  }

  public onDialogSubmit(modal: any, type: string, dismiss: boolean = false): void {
    if (dismiss) {
      if (type === 'newPastry' || type === 'editPastry') {
        this._newPastry.clear();
      } else if (type === 'newCategory' || type === 'editCategory') {
        this._newCategoryName = ''
        // enter and withdraw
      } else {
        if (this.clickedItem) {
          this.clickedItem.storageChange = 0;
        }
      }
      this._submitted = false;
      modal.close();
      return;
    }

    this._submitted = true;
    // new item
    if (type === 'newPastry' || type === 'editPastry') {
      if (!this.newPastry.readyForPost()) {
        return;
      }
      this.newPastry.materials = this.newPastry.materials.filter(x => x.amount);
      if (type === 'newPastry') {
        this.itemsService.postPastry(this.newPastry);
      } else {
        this.newPastry.pastryId = this.clickedItem.pastryId;
        this.itemsService.updatePastry(this.newPastry);
      }
      this._newPastry.clear();
      this._submitted = false;
    } else if (type === 'newCategory' || type === 'editCategory') {
      if (this._newCategoryName === '') {
        return;
      }
      const category = new PastryCategory(this._newCategoryName);
      if (type === 'newCategory') {
        this.itemsService.postCategory(category);
      } else {
        category.categoryId = this.clickedCategory.categoryId;
        this.itemsService.updateCategory(category);
      }
      this._newCategoryName = '';
      this._submitted = false;
    } else {
      if (!this.clickedItem.storageChange) {
        return;
      }
      // enter amount
      if (type === 'enter') {
        this.itemsService.enterAmount(this.clickedItem);
      // withdraw
      } else {
        this.itemsService.withdrawAmount(this.clickedItem);
      }
      this._submitted = false;
    }
    modal.close();
  }

  public deleteItem(modal: any, type: string): void {
    if (type === 'deletePastry') {
      this.itemsService.deletePastry(this.clickedItem);
    } else {
      this.itemsService.deleteCategory(this.clickedCategory);
    }
    modal.close();
  }

  public selectMaxAmount(index: number = -1): void {
    if (index !== -1) {
      this._itemList[index].storageChange = this._itemList[index].amount;
    } else {
      this._itemList[this._clickedIndex].storageChange = this._itemList[this._clickedIndex].amount;
    }
  }

  public selectAllMaxAmount(): void {
    this._itemList.forEach(
      item => {
        if (item.amount) {
          item.storageChange = item.amount;
        }
      }
    );
  }

  public storageEmpty(): boolean {
    return this._itemList.every(
      item => {
        return item.amount === 0;
      }
    );
  }

  public removeSelection(proceed: boolean = true): void {
    if (!proceed) { return; }

    this._itemList.forEach(
      item => {
        item.storageChange = 0;
    });
    this.reset[0] = {};
  }

  public storeItems(): void {
    this.itemsService.enterAmountList(this._itemList.filter(item => item.storageChange));
    this.removeSelection();
  }

  public pickItems(): void {
    this.itemsService.withdrawAmountList(this._itemList.filter(item => item.storageChange));
    this.removeSelection();
  }

  public anySelected(): boolean {
    return this._itemList.filter(item => item.storageChange).length > 0;
  }
}
