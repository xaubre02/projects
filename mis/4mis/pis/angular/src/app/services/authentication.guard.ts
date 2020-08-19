import { Injectable } from '@angular/core';
import { Router, CanActivate, ActivatedRouteSnapshot, RouterStateSnapshot } from '@angular/router';

import { AuthenticationService } from './authentication.service';

@Injectable({
  providedIn: 'root'
})
export class AuthenticationGuard implements CanActivate {
  public constructor(
    private router: Router,
    private authService: AuthenticationService
  ) {}

  public canActivate(route: ActivatedRouteSnapshot, state: RouterStateSnapshot) {
    // already logged in
    if (this.authService.loggedUser && this.authService.loggedUser.role !== 'customer') {
      return true;
    } else {
      this.router.navigate(['/login']);
      return false;
    }
  }
}
