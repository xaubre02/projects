import { Component, OnInit } from '@angular/core';
import { Router } from '@angular/router';

@Component({
  selector: 'app-footer',
  templateUrl: './footer.component.html',
  styleUrls: ['./footer.component.css']
})
export class FooterComponent implements OnInit {

  constructor(
    private router: Router
  ) {
  }

  ngOnInit(): void {
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
}
