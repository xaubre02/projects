import { Component, OnInit, Input } from '@angular/core';

@Component({
  selector: 'app-main-content',
  templateUrl: './main-content.component.html',
  styleUrls: ['./main-content.component.css']
})
export class MainContentComponent implements OnInit {
  @Input() noContentText: string = 'No content available';
  @Input() noContent: boolean = true;
  @Input() footer: boolean = false;
  @Input() loading: boolean = false;
  @Input() bgColor: string = null;
  private _footerClass: string = 'w-100 d-flex justify-content-end px-3 py-2';

  constructor() { }

  ngOnInit(): void { 
  }

  public get footerClass(): string {
    return this._footerClass;
  }
}
