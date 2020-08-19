import { Component, OnInit, OnDestroy } from '@angular/core';
import { NgbModal } from '@ng-bootstrap/ng-bootstrap';
import { Subscription } from 'rxjs';

import { AuthenticationService, UserService } from '../../services';
import { User } from '../../models';
import { userRolesDb } from '../../config';

@Component({
  selector: 'app-profile',
  templateUrl: './profile.component.html',
  styleUrls: ['./profile.component.css']
})
export class ProfileComponent implements OnInit, OnDestroy {
  private _submitted: boolean = false;
  private _passCheck: boolean = true;
  private _editedUser: User = new User();
  private _currentUser: User = new User();
  private _userDataSub: Subscription = new Subscription();

  constructor(
    private authService: AuthenticationService,
    private userService: UserService,
    private modalService: NgbModal
  ) { }

  ngOnInit(): void {
    this.loadUserData();
   }

  ngOnDestroy(): void {
    this._userDataSub.unsubscribe();
  }

  public get submitted(): boolean { return this._submitted; }
  public get passCheck(): boolean { return this._passCheck; }
  public get editedUser(): User   { return this._editedUser; }
  public get currentUser(): User  { return this._currentUser; }
  
  public get userCustomer(): boolean {
    if (this.authService.loggedUser) {
      return this.authService.loggedUser.role == userRolesDb.customer;
    }
    return false;
  }

  private loadUserData(): void {
    this._userDataSub.unsubscribe();

    if (this.authService.loggedUser.role === 'customer') {
      this._userDataSub = this.authService.getUser(this.authService.loggedUser.userId).subscribe(
        response => {
          if (response && this._currentUser !== response) {
            this._currentUser = response;
          } else if (this._currentUser !== response) {
            this._currentUser = new User();
          }
        }
      );
    } else {
      this._userDataSub = this.userService.getEmployee(this.authService.loggedUser.userId).subscribe(
        response => {
          if (response && this._currentUser !== response) {
            this._currentUser = response;
          } else if (this._currentUser !== response) {
            this._currentUser = new User();
          }
        }
      );
    }
  }

  public openDialog(dialog: any): void {
    this._editedUser.copy(this._currentUser);
    this.modalService.open(dialog, {windowClass: 'modalWindow'});
  }

  public onDialogSubmit(modal: any): void {
    this._passCheck = true;
    this._submitted = true;
    if (!this.editedUser.readyForPost()) {
      if (!this.editedUser.passwordsOK()) {
        this._passCheck = false;
      }
      return;
    }
    if (this.authService.loggedUser.role === 'customer') {
      this.authService.editUser(this.editedUser);
    } else {
      this.userService.updateEmployee(this.editedUser, true);
    }
    this._submitted = false;
    modal.close();
  }
}
