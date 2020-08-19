import { Injectable } from '@angular/core';

import { Pastry } from '../models';

@Injectable({
  providedIn: 'root'
})
export class CartService {
  private _items: Pastry[] = [];
  private _storageKey = 'cart';

  constructor() {
    let stored = JSON.parse(localStorage.getItem(this._storageKey));
    if (stored) {
      this._items = stored.map(x => new Pastry(x));
    }
  }

  public get items(): Pastry[] {
    return this._items;
  }

  public get totalCost(): number {
    let cost = 0;
    this._items.forEach(
      pastry => {
        cost += pastry.totalCost;
      }
    );

    return cost;
  }

  public addToCart(pastry: Pastry, quantity: number): void {
    const included = this._items.find(p => p.pastryId === pastry.pastryId);
    // item already in cart
    if (included) {
      included.quantity += quantity;
    } else {
      pastry.quantity = quantity;
      this._items.push(pastry);
    }
    localStorage.setItem(this._storageKey, JSON.stringify(this._items));
  }

  public updateQuantity(pastry: Pastry, quantity: number): void {
    this._items.find(p => p.pastryId === pastry.pastryId).quantity = quantity;
    localStorage.setItem(this._storageKey, JSON.stringify(this._items));
  }

  public removeFromCart(pastry: Pastry): void {
    this._items = this._items.filter(item => item.pastryId !== pastry.pastryId);
    localStorage.setItem(this._storageKey, JSON.stringify(this._items));
  }

  public clearCart(): void {
    this._items = [];
    localStorage.removeItem(this._storageKey);
  }
}
