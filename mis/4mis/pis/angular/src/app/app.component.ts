import { Component } from '@angular/core';
import { Router } from '@angular/router';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html'
})
export class AppComponent {

  public constructor(
    private router: Router
  ) {}

  public get loginPage(): boolean {
    return this.router.url.startsWith('/login');
  }

  public get registerPage(): boolean {
    return this.router.url.startsWith('/register');
  }
}
