import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';

@Component({
  selector: 'app-number-spinner',
  templateUrl: './number-spinner.component.html',
  styleUrls: ['./number-spinner.component.css']
})
export class NumberSpinnerComponent implements OnInit {
  @Input() private initValue: number;
  @Input() private minValue: number;
  @Input() private maxValue: number;
  @Input()
  public get value(): number {
    return this.counter;
  }
  public set value(val) {
    if (this.maxValue !== undefined && Number(val) >= this.maxValue) {
      this.counter = this.maxValue;
    } else {
      this.counter = Number(val);
    }
    this.valueChange.emit(this.counter);
  }
  @Output() valueChange: EventEmitter<number> = new EventEmitter<number>();
  private counter: number;

  constructor() { }

  ngOnInit(): void {
    if (this.initValue === undefined) {
      this.initValue = 0;
    }
    if (this.minValue === undefined) {
      this.minValue = 0;
    }
    this.counter = this.initValue;
  }

  public numberOnly(event: KeyboardEvent): boolean {
    const charCode = (event.which) ? event.which : event.keyCode;
    const isNumber = charCode === 46 || !(charCode > 31 && (charCode < 48 || charCode > 57));
    return isNumber && this.counter !== this.maxValue;
  }

  public increment(): void {
    if (this.maxValue === undefined || (this.maxValue !== undefined && this.value < this.maxValue)) {
      this.value++;
    }
  }

  public decrement(): void {
    if (this.value > this.minValue) {
      this.value--;
    }
  }

  public reachedLimit(): boolean {
    return this.value === this.minValue /*|| this.value === this.maxValue*/;
  }
}
