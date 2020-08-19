import { Component, OnInit, OnDestroy, ChangeDetectorRef, AfterViewChecked } from '@angular/core';
import { Subscription } from 'rxjs';
import { NgbModal } from '@ng-bootstrap/ng-bootstrap';

import { PastryOrderService, MaterialOrderService, UserService, AuthenticationService, PastryService, MaterialService } from '../../services';
import { PastryOrder, MaterialOrder, User, Pastry, Material } from '../../models';
import { userRolesDb } from '../../config';

@Component({
  selector: 'app-orders',
  templateUrl: './orders.component.html',
  styleUrls: ['./orders.component.css']
})
export class OrdersComponent implements OnInit, OnDestroy, AfterViewChecked {
  private _subscribed: Array<Subscription> = [];
  private _pastryOrderList: Array<PastryOrder> = [];
  private _customersList: Array<User> = [];
  private _pastryList: Array<Pastry> = [];
  private _selectedState: string = null;
  private _currentOrder: PastryOrder = new PastryOrder();
  
  private _materialOrderList: Array<MaterialOrder> = [];
  private _materialList: Array<Material> = [];
  private _deliveredOrder: boolean = null;
  private _currentMatOrder: MaterialOrder = new MaterialOrder();

  private _clickedIndex: number;
  private _submitted: boolean = false;

  private _dialogConfig = {
    type: '',
    title: '',
    buttonText: '',
    buttonClearText: '',
    buttonClass: '',
    confirmTitle: '',
    confirmButton: ''
  };

  constructor(
    private _pastryOrderService: PastryOrderService,
    private _materialOrderService:MaterialOrderService,
    private _userService: UserService,
    private _authService: AuthenticationService,
    private _pastryService: PastryService,
    private _materialService: MaterialService,
    private _modalService: NgbModal,
    private _cdr: ChangeDetectorRef
  ) {
    this._currentOrder.clear();
    this._currentMatOrder.clear();
  }

  ngOnInit(): void {
    this.loadData();
  }

  ngOnDestroy() {
    this._subscribed.forEach(
      subsriber => {
        subsriber.unsubscribe();
      }
    );
  }

  ngAfterViewChecked(): void {
    // Called after every check of the component's view. Applies to components only.
    // HOT FIX for ExpressionChangedAfterItHasBeenCheckedError
    this._cdr.detectChanges();
  }

  /*****************************************/
  /* Init methods */
  /*****************************************/
  private loadData(): void {
    this._subscribed.forEach(
      subscriber => {
        subscriber.unsubscribe();
      }
    );
    this._subscribed = [];

    this._subscribed.push(
      // load pastry orders
      this._pastryOrderService.getPastryOrders().subscribe(
        response => {
          if (response && response.length && this._pastryOrderList !== response) {
            this._pastryOrderList = response;
          } 
          else if (this._pastryOrderList !== response) {
            this._pastryOrderList = [];
          }
        }
      ),
      // load material orders
      this._materialOrderService.getMaterialOrders().subscribe(
        response => {
          if (response && response.length && this._materialOrderList !== response) {
            this._materialOrderList = response;
          } 
          else if (this._materialOrderList !== response) {
            this._materialOrderList = [];
          }
        }
      ),
      // load employees
      this._userService.getCustomers().subscribe(
        response => {
          if (response && this._customersList !== response && response.length) {
            this._customersList = response;
          } 
          else if (this._customersList !== response) {
            this._customersList = [];
          }
        }
      ),
      // load pastry
      this._pastryService.getPastries().subscribe(
        response => {
          if (response && this._pastryList !== response && response.length) {
            this._pastryList = response;
          } 
          else if (this._pastryList !== response) {
            this._pastryList = [];
          }
        }
      ),
      // load materials
      this._materialService.getMaterials().subscribe(
        response => {
          if (response && this._materialList !== response && response.length) {
            this._materialList = response;
          } 
          else if (this._materialList !== response) {
            this._materialList = [];
          }
        }
      )
    );
  }

  /*****************************************/
  /* Getters and setters */
  /*****************************************/

  // pastry orders stuff
  public get pastryOrderList(): Array<PastryOrder> { 
    return this._pastryOrderList; 
  }
  
  public get customersList(): Array<User> {
    return this._customersList;
  }

  public get pastryList(): Array<Pastry> {
    return this._pastryList;
  }

  public get selectedState(): string {
    return this._selectedState;
  }
  public set selectedState(value: string) {
    this._selectedState = value;
  }

  public get clickedOrder(): PastryOrder { 
    return this._pastryOrderList[this._clickedIndex]; 
  }
  
  public get currentOrder(): PastryOrder {
    return this._currentOrder;
  }
  public set currentOrder(value: PastryOrder) {
    this._currentOrder = value;
  }

  public couldBeEdited(order: PastryOrder): boolean {
    if (order.state === 'delivered' || order.state === 'canceled') {
      return false;
    }
    return true;
  }

  public trackByOrder(index: number, item: PastryOrder) {
    return item ? item.orderId : index;
  }
  

  // material orders stuff
  public get materialOrderList(): Array<MaterialOrder> {
    return this._materialOrderList;
  }
  
  public get materialList(): Array<Material> {
    return this._materialList;
  }

  public get clickedMaterialOrder(): MaterialOrder { 
    return this._materialOrderList[this._clickedIndex]; 
  }

  public get deliveredOrder(): boolean {
    return this._deliveredOrder;
  }
  public set deliveredOrder(value: boolean) {
    this._deliveredOrder = value;
  }
  
  public get currentMatOrder(): MaterialOrder {
    return this._currentMatOrder;
  }
  public set currentMatOrder(value: MaterialOrder) {
    this._currentMatOrder = value;
  }

  public trackByMaterialOrder(index: number, item: MaterialOrder) {
    return item ? item.orderId : index;
  }
  
  public isDelivered(order: MaterialOrder): boolean {
    if (order.delivered) {
      return true;
    }
    return false;
  }

  // general getters
  public get loading(): boolean {
    return (
      this._pastryOrderService.loadingPastryOrders ||
      this._materialOrderService.loadingMaterialOrders ||
      this._pastryService.loadingPastry ||
      this._userService.loadingCustomers
    ); 
  }
  
  public get dialogConfig() {
    return this._dialogConfig;
  }
  
  public get submitted(): boolean {
    return this._submitted;
  }

  public get userManager(): boolean {
    if (this._authService.loggedUser) {
      return this._authService.loggedUser.role == userRolesDb.manager;
    }
    return false;
  }

  public get userPastryStoreman(): boolean {
    if (this._authService.loggedUser) {
      return this._authService.loggedUser.role == userRolesDb.pastrystoreman;
    }
    return false;
  }

  public get userMaterialStoreman(): boolean {
    if (this._authService.loggedUser) {
      return this._authService.loggedUser.role == userRolesDb.materialstoreman;
    }
    return false;
  }

  public get userCustomer(): boolean {
    if (this._authService.loggedUser) {
      return this._authService.loggedUser.role == userRolesDb.customer;
    }
    return false;
  }
  

  /*****************************************/
  /* Pastry orders by state */
  /*****************************************/
  getStateCz(order: PastryOrder): string {
    if(!order) return '';
    
    switch (order.state) {
      case 'created': return 'Vytvořená';
      case 'prepared': return 'Připravená';
      case 'delivered': return 'Vyzvednutá';
      case 'canceled': return 'Zrušená';
      default: return '';
    }
  }

  loadOrdersByState(): void {
    if (!this._selectedState) {
      this._subscribed.push(
        this._pastryOrderService.getPastryOrders().subscribe()
      );
    }
    else {
      this._subscribed.push(
        this._pastryOrderService.getPastryOrdersByState(this._selectedState).subscribe()
      );
    }
  }

  /*****************************************/
  /* Material orders by state(delivered - true/false) */
  /*****************************************/
  getDeliveredCz(order: MaterialOrder): string {
    if (!order) return '';
    
    switch (order.delivered) {
      case true: return 'Přijatá';
      case false: return 'Aktivní';
      default: return '';
    }
  }

  loadMaterialOrdersByState(): void {
    if (this._deliveredOrder == null) {
      this._subscribed.push(
        this._materialOrderService.getMaterialOrders().subscribe()
      );
    }
    else {
      this._subscribed.push(
        this._materialOrderService.getMaterialOrdersByState(this._deliveredOrder).subscribe()
      );
    }
  }

  
  /*****************************************/
  /* Current pastry order(new/edit) methods */
  /*****************************************/
  public get availableCurrentOrderPastry(): Array<Pastry> {
    return this._pastryList.filter(x => !(this._currentOrder.items.find(i => i.pastryId === x.pastryId)));
  }

  public set currentOrderCustomerId(value: number) {
    if (this._currentOrder.customer) {
      this._currentOrder.customer.userId = value;
    }
  }
  public get currentOrderCustomerId(): number {
    if (this._currentOrder.customer) {
      return this._currentOrder.customer.userId;
    }
    return null;
  }

  public setCurrentOrderCustomer(): void {
    let customer: User = null;
    if (this.currentOrderCustomerId) {
      customer = this._customersList.find(usr => usr.userId == this.currentOrderCustomerId);
    }
    if (customer) {
      // save properties only, bug with shallow copy
      this._currentOrder.customer.email = customer.email;
      this._currentOrder.customer.name = customer.name;
      this._currentOrder.customer.surname = customer.surname;
      this._currentOrder.customer.email = customer.email;
    }
    else {
      // no user has been selected
      this._currentOrder.customer = new User();
    }
  }

  public selectCurrentOrderItem(id: number): void {
    let pastry = this._pastryList.find(x => x.pastryId === id);
    if (pastry && this._currentOrder.items.length) {
      // save property
      this._currentOrder.items[this._currentOrder.items.length - 1].cost = pastry.cost;
      this._currentOrder.items[this._currentOrder.items.length - 1].name = pastry.name;
    }
  }

  public setCurrentOrderPrice(item: Pastry): void {
    // HOT FIX with flag to ignore first valueChange of number-spinner component when editing order
    if (!item.allowEmitChange && this._dialogConfig.type == 'editOrder') {
      item.allowEmitChange = true;
      return;
    }

    let priceItems: number = 0;
    this._currentOrder.items.forEach(
      item => {
        if (item.count && item.cost) priceItems += (item.count * item.cost);
      }
    );
    this._currentOrder.price = priceItems;
  }

  public currentOrderAddPastry(): void {
    let pastry = new Pastry();
    if (this._pastryList.length) {
      pastry.pastryId = -1;
    }
    this._currentOrder.items.push(pastry);
  }

  public currentOrderRemovePastry(index: number): void {
    this._currentOrder.items.splice(index, 1);
  }


  /*****************************************/
  /* Current material order methods */
  /*****************************************/
  public get availableCurrentOrderMaterial(): Array<Material> {
    return this._materialList.filter(x => !(this._currentMatOrder.materials.find(m => m.materialId === x.materialId)));
  }

  public selectCurrentMatOrderMaterial(id: number): void {
    let material = this._materialList.find(x => x.materialId === id);
    if (material && this._currentMatOrder.materials.length) {
      // save property
      this._currentMatOrder.materials[this._currentMatOrder.materials.length - 1].cost = material.cost;
      this._currentMatOrder.materials[this._currentMatOrder.materials.length - 1].name = material.name;
    }
  }

  public currentMatOrderAddMaterial(): void {
    let material = new Material();
    if (this._materialList.length) {
      material.materialId = -1;
    }
    this._currentMatOrder.materials.push(material);
  }

  public currentMatOrderRemoveMaterial(index: number): void {
    this._currentMatOrder.materials.splice(index, 1);
  }

  
  /*****************************************/
  /* Modal dialog managing */
  /*****************************************/
  public openDialog(dialog: any, type: string, index?: number): void {
    // save index of clicked order
    this._clickedIndex = index;
    
    switch (type) {
      // new order dialog
      case 'newOrder':
        this._dialogConfig = {
          type: 'newOrder',
          title: 'Objednat pečivo (Krok 1/2)',
          buttonText: 'Pokračovat',
          buttonClass: 'btn-success',
          buttonClearText: 'Vyčistit',
          confirmTitle: 'Potvrzení objednávky (Krok 2/2)',
          confirmButton: 'Objednat'
        }
        break;
      // edit order dialog
      case 'editOrder':
        // "deep" copy
        this._currentOrder.copy(this.clickedOrder);

        this._dialogConfig = {
          type: 'editOrder',
          title: 'Upravit objednávku (Krok 1/2)',
          buttonText: 'Pokračovat',
          buttonClass: 'btn-success',
          buttonClearText: 'Zavřít',
          confirmTitle: 'Potvrdit změny (Krok 2/2)',
          confirmButton: 'Uložit změny'
        }
        break;
      // deliver order dialog
      case 'deliverOrder':
        // "deep" copy
        this._currentOrder.copy(this.clickedOrder);

        this._dialogConfig = {
          type: 'deliverOrder',
          title: 'Předat objednávku zákazníkovi',
          buttonText: 'Předat',
          buttonClass: 'btn-success',
          buttonClearText: 'Zrušit',
          confirmTitle: '',
          confirmButton: ''
        }
        break;
      // cancel order dialog
      case 'cancelOrder':
        // "deep" copy
        this._currentOrder.copy(this.clickedOrder);

        this._dialogConfig = {
          type: 'cancelOrder',
          title: 'Zrušit objednávku',
          buttonText: 'Ano',
          buttonClass: 'btn-danger',
          buttonClearText: 'Ne',
          confirmTitle: '',
          confirmButton: ''
        }
        break;
      // new material order dialog
      case 'newMatOrder':
        this._dialogConfig = {
          type: 'newMatOrder',
          title: 'Objednat suroviny (Krok 1/2)',
          buttonText: 'Pokračovat',
          buttonClass: 'btn-success',
          buttonClearText: 'Vyčistit',
          confirmTitle: 'Potvrzení objednávky (Krok 2/2)',
          confirmButton: 'Přidat objednávku'
        }
        break;
      // cancel material order dialog
      case 'acceptMatOrder':
        this._currentMatOrder = this.clickedMaterialOrder;

        this._dialogConfig = {
          type: 'acceptMatOrder',
          title: 'Přijmout objednávku',
          buttonText: 'Ano',
          buttonClass: 'btn-success',
          buttonClearText: 'Ne',
          confirmTitle: '',
          confirmButton: ''
        }
        break;
      // cancel material order dialog
      case 'cancelMatOrder':
        this._currentMatOrder = this.clickedMaterialOrder;

        this._dialogConfig = {
          type: 'cancelMatOrder',
          title: 'Smazat objednávku',
          buttonText: 'Ano',
          buttonClass: 'btn-danger',
          buttonClearText: 'Ne',
          confirmTitle: '',
          confirmButton: ''
        }
        break;
      default:
        this._dialogConfig = {
          type: '',
          title: '',
          buttonText: '',
          buttonClass: '',
          buttonClearText: '',
          confirmTitle: '',
          confirmButton: ''
        }
    }
    this._modalService.open(dialog, {windowClass: 'modalWindow'});
  }

  // clear data in modal dialog
  public onDialogClear(modal: any, type: string) {
    if (type === 'newOrder') {
      this._currentOrder.clear();
    }
    else if (type === 'editOrder') {
      modal.close();
    }
    else if (type === 'deliverOrder') {
      modal.close();
    }
    else if (type === 'cancelOrder') {
      modal.close();
    }
    else if (type === 'newMatOrder') {
      this._currentMatOrder.clear();
    }
    else if (type === 'acceptMatOrder') {
      modal.close();
    }
    else if (type === 'cancelMatOrder') {
      modal.close();
    }
    this._submitted = false;
  }

  // close and reinit data in modal dialog
  public onDialogClose(modal: any, type: string): void {
    // if (type === 'newOrder' || type === 'editOrder') {
    //   this._currentOrder.clear();
    // }
    this._currentOrder.clear();
    this._currentMatOrder.clear();
    this._submitted = false;
    modal.close();
  }

  // method invoked after submit button is clicked -> if clear 
  public onDialogSubmit(modal: any, dialogConfirm: any, type: string): void {
    this._submitted = true;

    if (type == 'newOrder') {
      this._currentOrder.state = 'created';
      if (this.userCustomer) {
        this._currentOrder.customer = this._authService.loggedUser;
      }
      if (!this._currentOrder.readyForPost()) {
        return;
      }
      modal.close();
      // open confirmation dialog
      this._modalService.open(dialogConfirm, {windowClass: 'modalWindow'});
    }
    else if (type == 'editOrder') {
      if (!this._currentOrder.readyForPut()) {
        return;
      }
      modal.close();
      // open confirmation dialog
      this._modalService.open(dialogConfirm, {windowClass: 'modalWindow'});
    }
    else if (type == 'deliverOrder') {
      if (!this._currentOrder.readyForPut()) {
        return;
      }
      // set state to delivered
      this._currentOrder.state = 'delivered';
      this._pastryOrderService.updatePastryOrder(this._currentOrder, true);
      this._currentOrder.clear();
      this._selectedState = null;
      modal.close();
    }
    else if (type == 'cancelOrder') {
      if (this.userCustomer) {
        this._currentOrder.customer = this._authService.loggedUser;
      }
      if (!this._currentOrder.orderId) {
        return;
      }
      this._pastryOrderService.cancelPastryOrder(this._currentOrder);
      this._currentOrder.clear();
      this._selectedState = null;
      modal.close();
    }
    else if (type == 'newMatOrder') {
      this._currentMatOrder.delivered = false;
      if (!this._currentMatOrder.readyForPost()) {
        return;
      }
      modal.close();
      // open confirmation dialog
      this._modalService.open(dialogConfirm, {windowClass: 'modalWindow'});
    }
    else if (type == 'acceptMatOrder') {
      if (!this._currentMatOrder.orderId) {
        return;
      }
      this._materialOrderService.acceptMaterialOrder(this._currentMatOrder);
      this._currentMatOrder.clear();
      this._deliveredOrder = null;
      modal.close();
    }
    else if (type == 'cancelMatOrder') {
      if (!this._currentMatOrder.orderId) {
        return;
      }
      this._materialOrderService.cancelMaterialOrder(this._currentMatOrder);
      this._currentMatOrder.clear();
      this._deliveredOrder = null;
      modal.close();
    }
    
    this._submitted = false;
  }

  // method invoked after user confirmed order
  onDialogConfirm(modal: any, type: string): void {
    if (type === 'newOrder') {
      this._currentOrder.items = this._currentOrder.items.filter(x => x.count);
      this._pastryOrderService.createPastryOrder(this._currentOrder);
      this._currentOrder.clear();
      this._selectedState = null;
    }
    else if (type == 'editOrder') {
      this._currentOrder.items = this._currentOrder.items.filter(x => x.count);
      this._pastryOrderService.updatePastryOrder(this._currentOrder);
      this._currentOrder.clear();
      this._selectedState = null;
    }
    else if (type == 'newMatOrder') {
      this._currentMatOrder.materials = this._currentMatOrder.materials.filter(x => x.amount);
      this._materialOrderService.createMaterialOrder(this._currentMatOrder);
      this._currentMatOrder.clear();
      this._deliveredOrder = null;
    }
    modal.close();
  }
}
