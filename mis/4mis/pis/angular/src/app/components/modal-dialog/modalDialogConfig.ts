export enum  ModalDialogType {
  confirmatory,
  informative
}

export class ButtonConfiguration {
  private _title: string;
  private _class: string;
  private _route: string;

  constructor(title: string, HTMLclass: string, route: string = null) {
    this._title = title;
    this._class = HTMLclass;
    this._route = route;
  }

  public get title(): string     { return this._title; }
  public get HTMLclass(): string { return this._class; }
  public get route(): string { return this._route; }
}

export class ModalDialogConfiguration {
  private _type: ModalDialogType;
  private _targetName: string;
  private _title: string;
  private _question: string;
  private _buttons: Array<ButtonConfiguration>;

  constructor(type: ModalDialogType, targetName: string, title: string, question: string, buttons: Array<ButtonConfiguration>) {
    this._type = type;
    this._targetName = targetName;
    this._title = title;
    this._question = question;
    this._buttons = buttons;
  }

  public get type(): ModalDialogType               { return this._type; }
  public get targetName(): string                  { return this._targetName; }
  public get isInformative(): boolean              { return this.type === ModalDialogType.informative; }
  public get isConfirmatory(): boolean             { return this.type === ModalDialogType.confirmatory; }
  public get title(): string                       { return this._title; }
  public get question(): string                    { return this._question; }
  public get buttons(): Array<ButtonConfiguration> { return this._buttons; }
}
