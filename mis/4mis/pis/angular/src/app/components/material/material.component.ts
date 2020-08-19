import { Component, OnInit, Input } from '@angular/core';

import { Material } from 'src/app/models';

@Component({
  selector: 'app-material',
  templateUrl: './material.component.html',
  styleUrls: ['./material.component.css']
})
export class MaterialComponent implements OnInit {
  @Input() private _material: Material;

  constructor() { }

  ngOnInit(): void {
    if (!this.material) {
      this._material = new Material();
    }
  }

  public get material(): Material {
    return this._material;
  }
}
