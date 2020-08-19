import { Component, OnInit, Input } from '@angular/core';

@Component({
  selector: 'app-side-menu',
  templateUrl: './side-menu.component.html',
  styleUrls: ['./side-menu.component.css']
})
export class SideMenuComponent implements OnInit {
  @Input() storageKey: string = 'storageKey';
  @Input() ignoreStorage: boolean = false;
  @Input() title: string = 'Menu Title';
  @Input() footer: boolean = false;
  @Input() defaultIndex: number = 0;
  private _btnClass: string = 'btn btn-light btn-block text-left font-weight-bold shadow-none';
  private _index: number;

  constructor() {}

  ngOnInit(): void {
    if (this.ignoreStorage) {
      this._index = this.defaultIndex;
    }
    else {
      this._index = JSON.parse(localStorage.getItem(this.storageKey)) || this.defaultIndex;
    }
  }

  public get buttonClass(): string {
    return this._btnClass;
  }

  public get index(): number {
    return this._index;
  }

  // return true if the index has changed its value, false otherwise
  public setIndex(index: number): boolean {
    const diff = index !== this._index;
    this._index = index;
    localStorage.setItem(this.storageKey, JSON.stringify(index));
    return diff;
  }
}
