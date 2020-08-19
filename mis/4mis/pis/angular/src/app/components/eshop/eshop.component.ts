import { Component, OnInit, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';

import { CartService, PastryService, AuthenticationService } from '../../services';
import { Pastry, PastryCategory, User } from '../../models';
import { ButtonConfiguration, ModalDialogConfiguration, ModalDialogType } from '../modal-dialog/modalDialogConfig';
import { userRolesDb } from 'src/app/config';

@Component({
  selector: 'app-eshop',
  templateUrl: './eshop.component.html',
  styleUrls: ['./eshop.component.css']
})
export class EshopComponent implements OnInit, OnDestroy {
  private _subscribed: Array<Subscription> = [];
  private _pastryList: Array<Pastry> = [];
  private _categoryList: Array<PastryCategory> = [];
  private _modalConfig: ModalDialogConfiguration;

  private loggedUser: User;
  
  constructor(
    private authService: AuthenticationService,
    private cartService: CartService,
    private pastryService: PastryService
  ) {
    this.authService.observableUser.subscribe(
      user => { 
        this.loggedUser = user; 
      }
    );

    this._modalConfig = new ModalDialogConfiguration(
      ModalDialogType.confirmatory,
      'itemConfirm',
      'Nákup',
      'Položka byla úspěšně přidána do košíku!',
      [
        new ButtonConfiguration('Zavřít', ''),
        new ButtonConfiguration('Do košíku', 'btn-success', '/eshop/cart'),
      ]);
   }

  ngOnInit(): void {
    this.loadPastry();
  }

  ngOnDestroy() {
    this._subscribed.forEach(
      subsriber => {
        subsriber.unsubscribe();
      }
    );
  }

  public get userIsAuthorized(): boolean {
    return this.loggedUser !== null && this.loggedUser.role !== 'customer';
  }
  public get userCustomer(): boolean {
    if (this.authService.loggedUser) {
      return this.authService.loggedUser.role == userRolesDb.customer;
    }
    return false;
  }

  public get categoryList(): Array<PastryCategory> { return this._categoryList; }
  public get pastryList(): Array<Pastry>           { return this._pastryList; }
  public pastryByCategory(categoryId: number): Array<Pastry> {
    if (categoryId === -1) {
      return this.pastryList;
    } else {
      return this.pastryList.filter(x => x.category.categoryId === categoryId)
    }
  }

  public loadPastry(): void {
    this._subscribed.forEach(
      subscriber => {
        subscriber.unsubscribe();
      }
    );
    this._subscribed = [];

    // load pastry categories
    this._subscribed.push(
      this.pastryService.getCategories().subscribe(
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
      this.pastryService.getPastries(false).subscribe(
        response => {
          if (response && this._pastryList !== response && response.length) {
            this._pastryList = response;
          } else if (this._pastryList !== response) {
            this._pastryList = [];
          }
        }
    ));
  }

  public buyItem(pastry: Pastry, quantity: number): void {
    if (quantity) {
      this.cartService.addToCart(pastry, quantity);
    }
  }

  public get modalConfig(): ModalDialogConfiguration {
    return this._modalConfig;
  }
}
