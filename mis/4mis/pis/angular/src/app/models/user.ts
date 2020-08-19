import { NgbDateStruct } from '@ng-bootstrap/ng-bootstrap';

import { userRoles } from '../config';

/** User */
export class User {
  private _birthDate: NgbDateStruct = null;
  public get birthDate(): NgbDateStruct {
    return this._birthDate;
  }
  public set birthDate(value: NgbDateStruct) {
    this._birthDate = value;
  }

  private _born: string;
  public get born(): string {
    if (this.birthDate !== null &&
        this.birthDate.hasOwnProperty('year') &&
        this.birthDate.hasOwnProperty('month') &&
        this.birthDate.hasOwnProperty('day')) {
      return String(this.birthDate.year) + '-' +
             String(this.birthDate.month).padStart(2, '0') + '-' +
             String(this.birthDate.day).padStart(2, '0');
    } else {
      return '';
    }
  }
  public set born(value: string) {
    const parts = value.split('-');
    this._birthDate = {
      year:  Number(parts[0]),
      month: Number(parts[1]),
      day:   Number(parts[2])
    };
  }
  public get bornCZ(): string {
    if (this.birthDate !== null &&
        this.birthDate.hasOwnProperty('year') &&
        this.birthDate.hasOwnProperty('month') &&
        this.birthDate.hasOwnProperty('day')) {
      return String(this.birthDate['day']) + '.' +
             String(this.birthDate['month']) + '.' +
             String(this.birthDate['year']);
    } else {
      return '';
    }
  }

  private _userId: number = null;
  public get userId(): number {
    return this._userId;
  }
  public set userId(value: number) {
    this._userId = value;
  }

  private _email: string;
  public get email(): string {
    return this._email;
  }
  public set email(value: string) {
    this._email = value;
  }

  private _password: string;
  public get password(): string {
    return this._password;
  }
  public set password(value: string) {
    this._password = value;
  }

  private _passwordRepeat: string;
  public get passwordRepeat(): string {
    return this._passwordRepeat;
  }
  public set passwordRepeat(value: string) {
    this._passwordRepeat = value;
  }

  private _token: string;
  public get token(): string {
    return this._token;
  }
  public set token(value: string) {
    this._token = value;
  }

  private _name: string;
  public get name(): string {
    return this._name;
  }
  public set name(value: string) {
    this._name = value;
  }

  private _surname: string;
  public get surname(): string {
    return this._surname;
  }
  public set surname(value: string) {
    this._surname = value;
  }

  private _role: string;
  public get role(): string {
    // set role
    if (this.selectedRole < Object.keys(userRoles).length) {
      this._role = Object.keys(userRoles)[this.selectedRole];
    }
    return this._role;
  }
  public set role(value: string) {
    // set selectedRole
    if (userRoles.hasOwnProperty(value)) {
      this._selectedRole = Object.keys(userRoles).indexOf(value);
    }
    this._role = value;
  }
  public get roleFormatted(): string {
    return userRoles.hasOwnProperty(this.role) ? userRoles[this.role] : '';
  }

  private _selectedRole: number = 0;
  public get selectedRole(): number {
    return Number(this._selectedRole);
  }
  public set selectedRole(value: number) {
    this._selectedRole = value;
  }

  // copy obj values
  public copy(obj: User): void {
    for (const key in obj) {
      if (obj.hasOwnProperty(key)) {
        this[key] = obj[key];
      }
    }
  }

  // clear object values
  public clear(): void {
    this.name = '';
    this.surname = '';
    this.email = '';
    this.birthDate = null;
    this.password = '';
    this.passwordRepeat = '';
  }

  public passwordsOK(): boolean {
    return this.password === this.passwordRepeat && this.password.length > 5;
  }

  // user is ready to be posted to the server
  public readyForPost(): boolean {
    return this.name !== '' &&
           this.surname !== '' &&
           this.email !== '' &&
           this.birthDate !== null &&
           this.birthDate.hasOwnProperty('year') &&
           this.birthDate.hasOwnProperty('month') &&
           this.birthDate.hasOwnProperty('day') &&
           this.passwordsOK();
  }

  // user is ready to be put to the server(case when manager does it, does not see passwords)
  public readyForPut(): boolean {
    return this.name !== '' &&
           this.surname !== '' &&
           this.email !== '' &&
           this.birthDate !== null &&
           this.birthDate.hasOwnProperty('year') &&
           this.birthDate.hasOwnProperty('month') &&
           this.birthDate.hasOwnProperty('day')
  }

  // new user
  public get apiNewUser(): object {
    return {
      born: this.born,
      email: this.email,
      password: this.password,
      name: this.name,
      surname: this.surname
    };
  }

  // edit user
  public get apiEditCustomer(): object {
    return {
      userId: String(this.userId),
      born: this.born,
      email: this.email,
      password: this.password,
      name: this.name,
      surname: this.surname,
      role: 'customer'
    };
  }

  // New employee
  public get apiNewEmployee(): object {
    return {
      born: this.born,
      email: this.email,
      password: this.password,
      name: this.name,
      surname: this.surname,
      role: this.role
    };
  }

  // Update an employee
  public get apiUpdateEmployee(): object {
    return {
      userId: this.userId,
      born: this.born,
      email: this.email,
      password: this.password,
      name: this.name,
      surname: this.surname,
      role: this.role
    };
  }
}
