import { Component, OnInit } from '@angular/core';
import { Router } from '@angular/router';

import { AuthenticationService } from '../../services';
import { User } from '../../models';

@Component({
  selector: 'app-register',
  templateUrl: './register.component.html',
  styleUrls: ['./register.component.css']
})
export class RegisterComponent implements OnInit {
  private _submitted: boolean = false;
  private _passCheck: boolean = true;
  private _newUser: User = new User();

  constructor(
    private router: Router,
    private authService: AuthenticationService
  ) {
    this.newUser.clear();
  }

  ngOnInit(): void { }

  public get loading(): boolean   { return this.authService.registering; }
  public get submitted(): boolean { return this._submitted; }
  public get passCheck(): boolean { return this._passCheck; }
  public get newUser(): User      { return this._newUser; }


  public onSubmit(): void {
    this._passCheck = true;
    this._submitted = true;
    if (!this.newUser.readyForPost()) {
      if (!this.newUser.passwordsOK()) {
        this._passCheck = false;
      }
      return;
    }

    this.authService.register(this.newUser);
    this._submitted = false;
    this.newUser.clear();
  }
}
