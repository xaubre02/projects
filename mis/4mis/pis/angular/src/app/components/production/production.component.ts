import { Component, OnInit, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';
import { NgbModal } from '@ng-bootstrap/ng-bootstrap';

import { AuthenticationService, ProductionPlanService, PastryService } from '../../services';
import { ProductionPlan, Pastry } from '../../models';
import { userRolesDb } from '../../config';

@Component({
  selector: 'app-production',
  templateUrl: './production.component.html',
  styleUrls: ['./production.component.css']
})
export class ProductionComponent implements OnInit {
  private _subscribed: Array<Subscription> = [];
  private _productionPlanList: Array<ProductionPlan> = [];
  private _pastryList: Array<Pastry> = [];
  private _todayPlan: ProductionPlan = null;

  private _proposalPlan: ProductionPlan = new ProductionPlan();
  private _newPlan: ProductionPlan = new ProductionPlan();
  private _editPlan: ProductionPlan = new ProductionPlan();

  private _clickedIndex: number;
  private _submitted: boolean = false;

  constructor(
    private _authService: AuthenticationService,
    private _productionPlanService: ProductionPlanService,
    private _pastryService: PastryService,
    private _modalService: NgbModal
  ) { 
    this._proposalPlan.clear();
    this._newPlan.clear();
    this._editPlan.clear();
  }

  ngOnInit(): void {
    this.loadData();
  }

  ngOnDestroy(): void {
    this._subscribed.forEach(
      subsriber => {
        subsriber.unsubscribe();
      }
    );
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
      // load production plans
      this._productionPlanService.getProductionPlans().subscribe(
        response => {
          if (response && response.length && this._productionPlanList !== response) {
            this._productionPlanList = response;
          } 
          else if (this._productionPlanList !== response) {
            this._productionPlanList = [];
          }
        }
      ),
      // load production plans for today
      this._productionPlanService.getTodayProductionPlan().subscribe(
        response => {
          if (response && this._todayPlan !== response) {
            console.log(response);
            this._todayPlan = response;
          } 
          else if (this._todayPlan !== response) {
            this._todayPlan = null;
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
      )
    );
  }
  
  /*****************************************/
  /* Getters and setters */
  /*****************************************/
  public get loading(): boolean {
    return (
      this._productionPlanService.loadingProductionPlans ||
      this._productionPlanService.loadingProductionPlansToday ||
      this._pastryService.loadingPastry
    ); 
  }

  public get loadingProposal(): boolean {
    return this._productionPlanService.loadingProductionPlanProposal; 
  }

  public get userIsAuthorized(): boolean {
    return this._authService.loggedUser !== null && this._authService.loggedUser.role !== 'customer';
  }
  public get userManager(): boolean {
    if (this._authService.loggedUser) {
      return this._authService.loggedUser.role == userRolesDb.manager;
    }
    return false;
  }

  public get productionPlanList(): Array<ProductionPlan> {
    return this._productionPlanList;
  }
  public set productionPlanList(value: Array<ProductionPlan>) {
    this._productionPlanList = value;
  }

  public get todayPlan(): ProductionPlan {
    return this._todayPlan;
  }
  public set todayPlan(value: ProductionPlan) {
    this._todayPlan = value;
  }

  public get pastryList(): Array<Pastry> {
    return this._pastryList;
  }

  public get proposalPlan(): ProductionPlan {
    return this._proposalPlan;
  }
  public set proposalPlan(value: ProductionPlan) {
    this._proposalPlan = value;
  }
  
  public get newPlan(): ProductionPlan {
    return this._newPlan;
  }
  public set newPlan(value: ProductionPlan) {
    this._newPlan = value;
  }
  
  public get editPlan(): ProductionPlan {
    return this._editPlan;
  }
  public set editPlan(value: ProductionPlan) {
    this._editPlan = value;
  }

  public get submitted(): boolean {
    return this._submitted;
  }

  public trackByPlan(index: number, item: ProductionPlan) {
    return item ? item.planId : index;
  }

  public get clickedPlan(): ProductionPlan { 
    return this._productionPlanList[this._clickedIndex]; 
  }

  /*****************************************/
  /* Modal proposal managing */
  /*****************************************/
  public openProposalPlanDialog(dialog: any): void {
    this._modalService.open(dialog, {windowClass: 'modalWindow'});
  }

  // close and reinit data in modal dialog
  public closeProposalPlan(modal: any): void {
    this._proposalPlan.clear();
    this._submitted = false;
    modal.close();
  }
  
  // method invoked after submit button is clicked
  public submitProposalPlan(modal: any, editDialog: any): void {
    this._submitted = true;

    if (!this._proposalPlan.readyForProposal) {
      return;
    }

    this._productionPlanService.getProposalProductionPlan(this._proposalPlan).subscribe(
      (response: ProductionPlan) => {
        if (response && response.planId) {
          this._proposalPlan.clear();
          modal.close();
          // save proposal
          this._editPlan.copy(response);
          // open edit dialog
          this._modalService.open(editDialog, {windowClass: 'modalWindow'});
        }
      },
      error => {
        modal.close();
      }
    );
    
    this._submitted = false;
  }


  /*****************************************/
  /* Edit plan managing */
  /*****************************************/
  public get availableEditPlanPastry(): Array<Pastry> {
    return this._pastryList.filter(x => !(this._editPlan.items.find(i => i.pastryId === x.pastryId)));
  }

  public selectEditPlanItem(id: number): void {
    let pastry = this._pastryList.find(x => x.pastryId === id);
    if (pastry && this._editPlan.items.length) {
      // save property
      this._editPlan.items[this._editPlan.items.length - 1].cost = pastry.cost;
      this._editPlan.items[this._editPlan.items.length - 1].name = pastry.name;
    }
  }

  public editPlanAddPastry(): void {
    let pastry = new Pastry();
    if (this._pastryList.length) {
      pastry.pastryId = -1;
    }
    this._editPlan.items.push(pastry);
  }

  public editPlanRemovePastry(index: number): void {
    this._editPlan.items.splice(index, 1);
  }


  public openEditPlanDialog(dialog: any, index: number): void {
    // save index of clicked order
    this._clickedIndex = index;
    // "deep" copy
    this._editPlan.copy(this.clickedPlan);

    this._modalService.open(dialog, {windowClass: 'modalWindow'});
  }

  public openEditTodayPlanDialog(dialog: any): void {
    // "deep" copy
    this._editPlan.copy(this.todayPlan);

    this._modalService.open(dialog, {windowClass: 'modalWindow'});
  }

  // clear data in modal dialog
  public clearEditPlan() {
    this._editPlan.clear();
    this._submitted = false;
  }

  // close and reinit data in modal dialog
  public closeEditPlan(modal: any): void {
    this._editPlan.clear();
    this._submitted = false;
    modal.close();
  }
  
  // method invoked after submit button is clicked -> if clear 
  public submitEditPlan(modal: any, confirmModal: any): void {
    this._submitted = true;
    if (!this._editPlan.readyForPut) {
      return;
    }
    
    this._submitted = false;
    modal.close();
    // open confirmation dialog
    this._modalService.open(confirmModal, {windowClass: 'modalWindow'});
  }

  // method for confirmation production plan creation
  public confirmEditPlan(modal: any): void {
    this._productionPlanService.putProductionPlan(this._editPlan);
    this._editPlan.clear();
    
    modal.close();
  }


  /*****************************************/
  /* New plan managing */
  /*****************************************/
  public get availableNewPlanPastry(): Array<Pastry> {
    return this._pastryList.filter(x => !(this._newPlan.items.find(i => i.pastryId === x.pastryId)));
  }

  public selectNewPlanItem(id: number): void {
    let pastry = this._pastryList.find(x => x.pastryId === id);
    if (pastry && this._newPlan.items.length) {
      // save property
      this._newPlan.items[this._newPlan.items.length - 1].cost = pastry.cost;
      this._newPlan.items[this._newPlan.items.length - 1].name = pastry.name;
    }
  }

  public newPlanAddPastry(): void {
    let pastry = new Pastry();
    if (this._pastryList.length) {
      pastry.pastryId = -1;
    }
    this._newPlan.items.push(pastry);
  }

  public newPlanRemovePastry(index: number): void {
    this._newPlan.items.splice(index, 1);
  }


  public openNewPlanDialog(dialog: any): void {
    this._modalService.open(dialog, {windowClass: 'modalWindow'});
  }

  // clear data in modal dialog
  public clearNewPlan() {
    this._newPlan.clear();
    this._submitted = false;
  }

  // close and reinit data in modal dialog
  public closeNewPlan(modal: any): void {
    this._newPlan.clear();
    this._submitted = false;
    modal.close();
  }
  
  // method invoked after submit button is clicked -> if clear 
  public submitNewPlan(modal: any, confirmModal: any): void {
    this._submitted = true;
    if (!this._newPlan.readyForPost) {
      return;
    }
    
    this._submitted = false;
    modal.close();
    // open confirmation dialog
    this._modalService.open(confirmModal, {windowClass: 'modalWindow'});
  }

  // method for confirmation production plan creation
  public confirmNewPlan(modal: any): void {
    this._productionPlanService.postProductionPlan(this._newPlan);
    this._newPlan.clear();
    
    modal.close();
  }
}
