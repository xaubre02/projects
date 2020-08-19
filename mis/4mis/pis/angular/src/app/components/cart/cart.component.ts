import { Component, OnInit, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';

import { CartService, AuthenticationService, PastryOrderService } from 'src/app/services';
import { Pastry, User, PastryOrder } from '../../models';
import { ButtonConfiguration, ModalDialogConfiguration, ModalDialogType } from '../modal-dialog/modalDialogConfig';
import { userRolesDb } from 'src/app/config';

@Component({
  selector: 'app-cart',
  templateUrl: './cart.component.html',
  styleUrls: ['./cart.component.css']
})
export class CartComponent implements OnInit, OnDestroy {
  private _subscribed: Array<Subscription> = [];
  private _itemList: Pastry[] = [];
  private _itemToBeRemoved: Pastry;
  private _modalConfigs: Array<ModalDialogConfiguration> = [];

  private _loggedUser: User;
  private _newOrder: PastryOrder = new PastryOrder();
  
  constructor(
    private authService: AuthenticationService,
    private cartService: CartService,
    private _orderService: PastryOrderService
  ) {
    this.authService.observableUser.subscribe(
      user => {
        if (user && user.userId) {
          // tmp save
          this.loggedUser = user;

          // get user with all properties
          this.authService.getUser(user.userId).subscribe(
            (userObject: User) => {
              // save user object with all properties
              if (userObject) {
                this.loggedUser = userObject;
              }
            }
          )
        }
      }
    );

    this._modalConfigs.push(new ModalDialogConfiguration(
      ModalDialogType.confirmatory,
      'itemRemove',
      'Odstranit položku',
      'Opravdu si přejete odstranit danou položku z košíku?',
      [
        new ButtonConfiguration('Zrušit', ''),
        new ButtonConfiguration('Odstranit', 'btn-danger'),
      ]));
    this._modalConfigs.push(new ModalDialogConfiguration(
      ModalDialogType.confirmatory,
      'clearCart',
      'Vysypat košík',
      'Opravdu si přejete odstranit všechny položky z košíku?',
      [
        new ButtonConfiguration('Zrušit', ''),
        new ButtonConfiguration('Odstranit', 'btn-danger'),
      ]));
   }

  ngOnInit(): void {
    this._itemList = this.cartService.items;
    this._newOrder.clear();
  }

  ngOnDestroy() {
    this._subscribed.forEach(
      subsriber => {
        subsriber.unsubscribe();
      }
    );
  }
  
  
  public get itemList(): Pastry[] { return this._itemList; }
  public get itemLabel(): string {
    if (this.itemList.length === 1) {
      return 'položka';
    } else if (this.itemList.length > 1 && this.itemList.length < 5) {
      return 'položky';
    } else {
      return 'položek';
    }
  }

  public get totalCost(): number { return this.cartService.totalCost; }
  public set itemToBeRemoved(item: Pastry) { this._itemToBeRemoved = item; }

  public get loggedUser(): User {
    return this._loggedUser;
  }
  public set loggedUser(value: User) {
    this._loggedUser = value;
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
  
  public get newOrder(): PastryOrder {
    return this._newOrder;
  }
  public set newOrder(value: PastryOrder) {
    this._newOrder = value;
  }

  public removeItem(item?: Pastry): void {
    if (!item) {
      item = this._itemToBeRemoved;
    }
    this.cartService.removeFromCart(item);
    this._itemList = this._itemList.filter(pastry => pastry.pastryId !== item.pastryId);
  }

  public clearCart(): void {
    this.cartService.clearCart();
    this._itemList = [];
  }

  public onQuantityChange(item: Pastry, quantity: number): void {
    if (quantity) {
      this.cartService.updateQuantity(item, quantity);
    }
  }

  public get modalConfigs(): Array<ModalDialogConfiguration> {
    return this._modalConfigs;
  }


  /*******************************************/
  /* State of cart managing */
  /*******************************************/
  public readonly CART01: number = 0;
  public readonly CART02Auth: number = 1;
  public readonly CART02NonAuth: number = 2;
  public readonly CART03: number = 3;
  public readonly CART04: number = 4;

  public submitted01: boolean = false;
  public submitted02Auth: boolean = false;

  private _cartState: number = this.CART01;
  public get cartState(): number {
    return this._cartState;
  }
  public set cartState(value: number) {
    this._cartState = value;
  }

  private _termsConditions: boolean = false;
  public get termsConditions(): boolean {
    return this._termsConditions;
  }
  public set termsConditions(value: boolean) {
    this._termsConditions = value;
  }

  continueToState02(): void {
    // already authenticated
    if (this.userCustomer) {
      this.submitted01 = true;
      
      if (this._itemList.length > 0 && this._itemList.filter(x => x.quantity).length == this._itemList.length) {
        this._cartState = this.CART02Auth;
        this.submitted01 = false;
      }
    }
    else {
      this._cartState = this.CART02NonAuth;
    }
  }
  
  continueToState03(): void {
    if (this.userCustomer) {
      this.submitted02Auth = true;

      if (this._newOrder.isTomorrowOrLater && this._termsConditions) {
        this._cartState = this.CART03;
        this.submitted02Auth = false;
      }
    }
  }

  createNewOrder(): void {
    // save divided data to new order
    this.newOrder.customer = this.loggedUser;
    this.newOrder.state = 'created';
    this.newOrder.price = this.totalCost;
    
    this.itemList.forEach(
      item => {
        item.count = item.quantity;
      }
    );
    this.newOrder.items = this.itemList;

    // post
    this._orderService.createPastryOrder(this.newOrder);

    this.clearCart();
    this.newOrder.clear();
    this._cartState = this.CART04;
  }
}
