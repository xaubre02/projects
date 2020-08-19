import { Component, OnInit } from '@angular/core';
import { Router, ActivatedRoute } from '@angular/router';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';
import { first } from 'rxjs/operators';

import { AuthenticationService } from '../../services';


@Component({
  templateUrl: 'login.component.html',
  styleUrls: ['login.component.css']
})
export class LoginComponent implements OnInit {
  private _loading: boolean;
  private _incorrect: boolean;
  private _submitted: boolean;
  private _loginForm: FormGroup;
  private _returnUrl: string;

  public constructor(
    private route: ActivatedRoute,
    private router: Router,
    private formBuilder: FormBuilder,
    private authService: AuthenticationService
  ) {
    this._loading = false;
    this._incorrect = false;
    this._submitted = false;

    // redirect to home if already logged in
    if (this.authService.loggedUser) {
      this.router.navigate(['/']);
    }
  }

  public ngOnInit() {
    this._loginForm = this.formBuilder.group({
      username: ['', Validators.required],
      password: ['', Validators.required]
    });

    // get return url from route parameters or default to '/'
    this._returnUrl = this.route.snapshot.queryParams.returnUrl || '/eshop';
  }

  public get loading(): boolean     { return this._loading; }
  public get submitted(): boolean   { return this._submitted; }
  public get incorrect(): boolean   { return this._incorrect; }
  public get formGroup(): FormGroup { return this._loginForm; }
  public get form(): any            { return this._loginForm.controls; }

  public onSubmit(): void {
    this._submitted = true;
    this._incorrect = false;

    // invalid form
    if (this._loginForm.invalid) {
      return;
    }

    this._loading = true;
    this.authService.login(this._loginForm.value.username, this._loginForm.value.password)
      .pipe(first())
      .subscribe(
        _ => {
          this.router.navigate([this._returnUrl]);
        },
        error => {
          console.log(error);
          this._loading = false;
          this._incorrect = true;
          this._submitted = false;
        });
  }
}
