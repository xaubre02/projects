import { NgModule } from '@angular/core';
import { NgbModule } from '@ng-bootstrap/ng-bootstrap';
import { BrowserModule } from '@angular/platform-browser';
import { HttpClientModule } from '@angular/common/http';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { RouterModule } from '@angular/router';
import { DatePipe } from '@angular/common';

// custom components
import { AppComponent } from './app.component';
import { AuthenticationGuard } from './services';
import {
  AlertComponent,
  CartComponent,
  DatePickerComponent,
  EshopComponent,
  ExpeditionComponent,
  HomeComponent,
  LoginComponent,
  MainContentComponent,
  ManagementComponent,
  MaterialComponent,
  ModalDialogComponent,
  NumberSpinnerComponent,
  OrdersComponent,
  ProfileComponent,
  RegisterComponent,
  SideMenuComponent,
  StorageComponent,
  TopBarComponent
} from './components';
import { ProductionComponent } from './components/production/production.component';
import { FooterComponent } from './components/footer/footer.component';

@NgModule({
  declarations: [
    AppComponent,
    AlertComponent,
    CartComponent,
    DatePickerComponent,
    EshopComponent,
    ExpeditionComponent,
    HomeComponent,
    LoginComponent,
    MainContentComponent,
    ManagementComponent,
    MaterialComponent,
    ModalDialogComponent,
    NumberSpinnerComponent,
    OrdersComponent,
    ProfileComponent,
    RegisterComponent,
    SideMenuComponent,
    StorageComponent,
    TopBarComponent,
    OrdersComponent,
    ProductionComponent,
    FooterComponent
  ],
  imports: [
    NgbModule,
    BrowserModule,
    FormsModule,
    HttpClientModule,
    ReactiveFormsModule,
    RouterModule.forRoot([
      { path: '', component: HomeComponent, canActivate: [AuthenticationGuard] },
      { path: 'storage', component: StorageComponent, canActivate: [AuthenticationGuard] },
      { path: 'expedition', component: ExpeditionComponent, canActivate: [AuthenticationGuard] },
      { path: 'management', component: ManagementComponent, canActivate: [AuthenticationGuard] },
      { path: 'orders', component: OrdersComponent, canActivate: [AuthenticationGuard] },
      { path: 'production', component: ProductionComponent, canActivate: [AuthenticationGuard] },
      { path: 'eshop', component: EshopComponent},
      { path: 'eshop/cart', component: CartComponent},
      { path: 'eshop/orders', component: OrdersComponent},
      { path: 'eshop/profile', component: ProfileComponent},
      { path: 'login', component: LoginComponent},
      { path: 'register', component: RegisterComponent},
      // default: home
      { path: '**', redirectTo: '' }
    ])
  ],
  exports: [RouterModule],
  providers: [DatePipe],
  bootstrap: [AppComponent]
})

export class AppModule { }
