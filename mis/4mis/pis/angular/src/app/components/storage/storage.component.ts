import { Component, OnInit, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';
import { NgbModal } from '@ng-bootstrap/ng-bootstrap';

import { MaterialService, AuthenticationService } from '../../services';
import { Material } from '../../models';
import { userRolesDb } from '../../config';

@Component({
  selector: 'app-storage',
  templateUrl: './storage.component.html',
  styleUrls: ['./storage.component.css']
})
export class StorageComponent implements OnInit, OnDestroy {
  private _subscribed: Array<Subscription> = [];
  private _itemList: Array<Material>;
  private _submitted: boolean = false;
  private _newItemName: string = '';
  private _clickedIndex: number;
  private _dialogConfig = {
    type: '',
    title: '',
    buttonText: '',
    buttonClass: ''
  }

  public reset: any[] = [{}];

  constructor(
    private authService: AuthenticationService,
    private itemsService: MaterialService,
    private modalService: NgbModal
  ) { }

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

  public get itemList(): Array<Material> { return this._itemList; }
  public get submitted(): boolean        { return this._submitted; }
  public get newItemName(): string       { return this._newItemName; }
  public set newItemName(value: string)  { this._newItemName = value; }
  public get dialogConfig()              { return this._dialogConfig; }
  public get loadingItems(): boolean     { return this.itemsService.loadingMaterial; }
  public get loadingAmount(): boolean    { return this.itemsService.loadingAmount; }
  public get clickedItem(): Material     { return this._itemList[this._clickedIndex]; }

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
  public get userCustomer(): boolean {
    if (this.authService.loggedUser) {
      return this.authService.loggedUser.role == userRolesDb.customer;
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

    this._subscribed.push(
      this.itemsService.getMaterials().subscribe(
        response => {
          if (response && this._itemList !== response && response.length) {
            this._itemList = response;
          } else if (this._itemList !== response) {
            this._itemList = [];
          }
        }
    ));
  }

  public trackByFunction(index: number, item: Material) {
    return item ? item.materialId : index;
  }

  public openDialog(dialog: any, type: string, index?: number): void {
    this._clickedIndex = index;
    switch (type) {
      // new material dialog
      case 'new':
        this._dialogConfig = {
          type: 'new',
          title: 'Přidat novou surovinu',
          buttonText: 'Přidat',
          buttonClass: 'btn-success'
        }
        break;
      // edit material dialog
      case 'edit':
        this.newItemName = this.clickedItem.name;
        this._dialogConfig = {
          type: 'edit',
          title: 'Upravit surovinu \'' + this.newItemName + '\'',
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
      default:
        this._dialogConfig = {
          type: 'withdraw',
          title: 'Vyskladnit: ' + this.clickedItem.name,
          buttonText: 'Vyskladnit',
          buttonClass: 'btn-danger'
        }
    }
    this.modalService.open(dialog, {windowClass: 'modalWindow'});
  }

  public onDialogSubmit(modal: any, type: string, dismiss: boolean = false): void {
    if (dismiss) {
      if (type === 'new' || type === 'edit') {
        this._newItemName = '';
      } else {
        this.clickedItem.storageChange = 0;
      }
      this._submitted = false;
      modal.close();
      return;
    }

    this._submitted = true;
    // new item
    if (type === 'new' || type === 'edit') {
      if (this.newItemName === '') {
        return;
      }
      const material = new Material(this.newItemName);
      if (type === 'new') {
        this.itemsService.postMaterial(material);
      } else {
        material.materialId = this.clickedItem.materialId;
        this.itemsService.updateMaterial(material);
      }
      this._newItemName = '';
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

  public deleteMaterial(modal: any): void {
    this.itemsService.deleteMaterial(this.clickedItem);
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
