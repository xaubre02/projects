import { Component, OnInit } from '@angular/core';
import { Router } from '@angular/router';

import { User } from '../../models';
import { AuthenticationService, CartService } from '../../services';
import { userRolesDb } from '../../config';

@Component({
  selector: 'app-top-bar',
  templateUrl: './top-bar.component.html',
  styleUrls: ['./top-bar.component.css']
})
export class TopBarComponent implements OnInit {
  private loggedUser: User;

  constructor(
    private authService: AuthenticationService,
    private cartService: CartService,
    private router: Router
  ) {
    this.authService.observableUser.subscribe(user => { this.loggedUser = user; });
  }

  ngOnInit(): void {
  }

  public get userIsAuthorized(): boolean {
    return this.loggedUser !== null && this.loggedUser.role !== 'customer';
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

  public get cartItemsCount(): number {
    return this.cartService.items.length;
  }

  public get eshopPage(): boolean {
    return this.router.url.startsWith('/eshop');
  }

  public get allowEshop(): boolean {
    return (
      this.router.url.startsWith('/eshop/orders') ||
      this.router.url.startsWith('/eshop/cart') ||
      this.router.url.startsWith('/eshop/profile')
    );
  }

  public get homePage(): boolean {
    return (this.router.url.startsWith('/home') || this.router.url == '' || this.router.url == '/');
  }

  public get storagePage(): boolean {
    return this.router.url.startsWith('/storage');
  }

  public get expeditionPage(): boolean {
    return this.router.url.startsWith('/expedition');
  }

  public get productionPage(): boolean {
    return this.router.url.startsWith('/production');
  }

  public get managementPage(): boolean {
    return this.router.url.startsWith('/management');
  }

  public get ordersPage(): boolean {
    return this.router.url.startsWith('/orders');
  }

  public get userIsLogged(): boolean {
    return this.authService.loggedUser !== null;
  }

  public login(): void {
    // redirect to the previous URL after the login
    this.router.navigate(['/login'], { queryParams: { returnUrl: this.router.routerState.snapshot.url }});
  }

  public logout(): void {
    this.authService.logout();
    if (this.allowEshop) {
      this.router.navigate(['/eshop']);
    }
    else if (!this.eshopPage) {
      this.router.navigate(['/login']);
    }
  }
}
