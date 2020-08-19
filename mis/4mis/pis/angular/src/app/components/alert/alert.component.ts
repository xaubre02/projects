import { Component, OnInit, OnDestroy, ViewChild } from '@angular/core';
import { Router } from '@angular/router';
import { Subscription } from 'rxjs';
import { NgbModal } from '@ng-bootstrap/ng-bootstrap';

import { AlertService } from '../../services'

@Component({
  selector: 'app-alert',
  templateUrl: './alert.component.html',
  styleUrls: ['./alert.component.css']
})
export class AlertComponent implements OnInit, OnDestroy {
  private _subscription: Subscription;
  private _alert: any;

  @ViewChild('modalDialog') private modalDialog: any;

  constructor(
    private router: Router,
    private alertService: AlertService,
    private modalService: NgbModal
  ) { }

  ngOnInit(): void {
    this._subscription = this.alertService.getAlert().subscribe(
      alert => {
        this.alert = alert;
    });
  }

  ngOnDestroy() {
    this._subscription.unsubscribe();
  }

  public get alert(): any {
    return this._alert;
  }

  public set alert(value) {
    this._alert = value;
    if (this.alert) {
      this.modalService.open(this.modalDialog, {windowClass: 'modalWindow'});
    }
  }

  public get color(): string {
    switch (this.alert.type) {
      case 'success':  return 'text-success';
      case 'warning':  return 'text-warning';
      case 'error':    return 'text-danger';
      default:         return '';
    }
  }

  public get icon(): string {
    switch (this.alert.type) {
      case 'success':  return 'fas fa-check';
      case 'warning':  return 'fas fa-exclamation-triangle';
      case 'error':    return 'fas fa-exclamation-triangle';
      default:         return '';
    }
  }

  public get title(): string {
    switch (this.alert.type) {
      case 'success':  return 'Úspěch';
      case 'warning':  return 'Upozornění';
      case 'error':    return 'Chyba!';
      default:         return '';
    }
  }

  public processClose(modal: any): void {
    modal.close();
    if (this.alert.navigate !== null) {
      this.router.navigate([this.alert.navigate]);
    }
  }
}
