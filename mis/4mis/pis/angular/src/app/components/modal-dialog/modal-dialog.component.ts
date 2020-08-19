import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';
import { ButtonConfiguration, ModalDialogConfiguration, ModalDialogType } from './modalDialogConfig';

@Component({
  selector: 'app-modal-dialog',
  templateUrl: './modal-dialog.component.html',
  styleUrls: ['./modal-dialog.component.css']
})
export class ModalDialogComponent implements OnInit {
  @Input() private _config: ModalDialogConfiguration;
  @Output() private _confirm: EventEmitter<any> = new EventEmitter();

  constructor() { }

  ngOnInit(): void {
    if (!this._config) {
      this._config = new ModalDialogConfiguration(ModalDialogType.informative, '', '', '', [new ButtonConfiguration('Zavřít', '')]);
    }
  }

  public get config(): ModalDialogConfiguration { return this._config; }

  public confirm(): void {
    this._confirm.emit();
  }
}
