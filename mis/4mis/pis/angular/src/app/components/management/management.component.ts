import { Component, OnInit, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';
import { NgbModal } from '@ng-bootstrap/ng-bootstrap';

import { AuthenticationService, UserService } from '../../services';
import { User } from '../../models';
import { userRoles } from '../../config';
import { userRolesDb } from '../../config';

@Component({
  selector: 'app-management',
  templateUrl: './management.component.html',
  styleUrls: ['./management.component.css']
})
export class ManagementComponent implements OnInit, OnDestroy {
  private _subscribed: Array<Subscription> = [];
  private _itemList: Array<User>;
  private _customerList: Array<User> = [];
  private _selectedRole: number = -1;
  private _submitted: boolean = false;
  private _passCheck: boolean = true;
  private _clickedIndex: number;
  private _dialogConfig = {
    type: '',
    title: '',
    buttonText: '',
    buttonClass: ''
  }
  private _newUser: User = new User();

  constructor(
    private authService: AuthenticationService,
    private itemsService: UserService,
    private modalService: NgbModal,
  ) {
    this.newUser.clear();
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

  
  public get customerList(): Array<User> {
    return this._customerList;
  }
  public set customerList(value: Array<User>) {
    this._customerList = value;
  }
  public get itemList(): Array<User> {
    if (this.selectedRole === -1) {
      return this._itemList;
    } else {
      return this._itemList.filter(x => x.role === Object.keys(userRoles)[this.selectedRole]);
    }
  }
  public get roleList(): object { 
    return Object.values(userRoles); 
  }
  public get selectedRole(): number      { return Number(this._selectedRole); }
  public set selectedRole(value: number) { this._selectedRole = value; }
  public get submitted(): boolean        { return this._submitted; }
  public get passCheck(): boolean        { return this._passCheck; }
  public get dialogConfig()              { return this._dialogConfig; }
  public get loadingItems(): boolean     { return this.itemsService.loadingEmployees || this.itemsService.loadingCustomers; }
  public get newUser(): User             { return this._newUser; }
  private clickedUser(isEmployee: boolean): User {
    if (isEmployee) {
     return this.itemList[this._clickedIndex];
    }
    else {
      return this._customerList[this._clickedIndex];
    }
  }

  public get userManager(): boolean {
    if (this.authService.loggedUser) {
      return this.authService.loggedUser.role == userRolesDb.manager;
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

    // load employees
    this._subscribed.push(
      this.itemsService.getEmployees().subscribe(
        response => {
          if (response && this._itemList !== response && response.length) {
            this._itemList = response;
          } 
          else if (this._itemList !== response) {
            this._itemList = [];
          }
        }
      ),
      this.itemsService.getCustomers().subscribe(
        response => {
          if (response && this._customerList !== response && response.length) {
            this._customerList = response;
          } 
          else if (this._customerList !== response) {
            this._customerList = [];
          }
        }
      )
    );
  }

  public trackByUser(index: number, item: User) {
    return item ? item.userId : index;
  }

  public openDialog(dialog: any, type: string, index?: number, isEmployee?: boolean): void {
    this._submitted = false;
    this._clickedIndex = index;
    switch (type) {
      // new employee dialog
      case 'newUser':
        // default values
        this._dialogConfig = {
          type: 'newUser',
          title: 'Přidat nového uživatele',
          buttonText: 'Přidat',
          buttonClass: 'btn-success'
        }
        break;
      // edit employee dialog
      case 'editUser':
        // default values
        this.newUser.copy(this.clickedUser(isEmployee));
        this._dialogConfig = {
          type: 'editUser',
          title: 'Upravit uživatele ' + this.newUser.name + ' ' + this.newUser.surname,
          buttonText: 'Upravit',
          buttonClass: 'btn-primary'
        }
        break;
      default:
        break;
    }
    this.modalService.open(dialog, {windowClass: 'modalWindow'});
  }

  public onDialogSubmit(modal: any, type: string, dismiss: boolean = false): void {
    if (dismiss) {
      this.newUser.clear();
      this._submitted = false;
      modal.close();
      return;
    }

    this._passCheck = true;
    this._submitted = true;
    
    if (type === 'newUser') {
      if (!this.newUser.readyForPost() ) {
        if (!this.newUser.passwordsOK()) {
          this._passCheck = false;
        }
        return;
      }

      if (this.newUser.role == userRolesDb.customer) {
        this.itemsService.postCustomer(this.newUser);
      }
      else {
        this.itemsService.postEmployee(this.newUser);
      }
    }
    else if (type === 'editUser') {
      if (!this.newUser.readyForPut() ) {
        return;
      }
      // cannot update customer
      // if (this.newUser.role == userRolesDb.customer) {
      //   this.itemsService.updateCustomer(this.newUser);
      // }
      // else {
      this.itemsService.updateEmployee(this.newUser);
    }
    
    this.newUser.clear();
    this._submitted = false;
    modal.close();
  }
}
