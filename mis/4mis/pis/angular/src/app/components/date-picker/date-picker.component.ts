import { Component, OnInit, Input, Output, EventEmitter, Injectable, ViewChild, ChangeDetectorRef } from '@angular/core';
import { NgbCalendar, NgbDateStruct, NgbDateParserFormatter } from '@ng-bootstrap/ng-bootstrap';

@Injectable()
export class CzechDateParserFormatter extends NgbDateParserFormatter {
  readonly DELIMITER_SLASH = '/';
  readonly DELIMITER_DASH = '-';
  readonly DELIMITER_DOT = '.';

  public parse(value: string): NgbDateStruct {
    let result: NgbDateStruct = null;
    if (value) {
      let date: string[];
      // slash-separated
      if (value.search(this.DELIMITER_SLASH) !== -1) {
        date = value.split(this.DELIMITER_SLASH);
      // dash-separated
      } else if (value.search(this.DELIMITER_DASH) !== -1) {
        date = value.split(this.DELIMITER_DASH);
      // dot-separated
      } else {
        date = value.split(this.DELIMITER_DOT);
      }
      result = {
        day  : parseInt(date[0], 10),
        month: parseInt(date[1], 10),
        year : parseInt(date[2], 10)
      };
    }
    return result;
  }

  public format(date: NgbDateStruct): string {
    let result: string = null;
    if (date) {
      result = date.day + this.DELIMITER_DOT + date.month + this.DELIMITER_DOT + date.year;
    }
    return result;
  }
}

@Component({
  selector: 'app-date-picker',
  templateUrl: './date-picker.component.html',
  styleUrls: ['./date-picker.component.css'],
  providers: [{ provide: NgbDateParserFormatter, useClass: CzechDateParserFormatter }]
})
export class DatePickerComponent implements OnInit {
  private _date: NgbDateStruct;
  @Input()
  public get date(): NgbDateStruct {
    return this._date;
  }
  public set date(val: NgbDateStruct) {
    this._date = val;
    this.dateChange.emit(this._date);
  }
  @Output() dateChange: EventEmitter<NgbDateStruct> = new EventEmitter<NgbDateStruct>();

  @Input() minDate: NgbDateStruct;
  @Input() maxDate: NgbDateStruct;
  @Input() startDate: NgbDateStruct;
  @Input() submitted: boolean = true;
  @Input() tomorrowValidation: boolean = false;
  @Input() disableWeekend: boolean = false;

  @ViewChild('calendar') private _calendar: any;

  constructor(
    private ngbCalendar: NgbCalendar,
    private cdr: ChangeDetectorRef
  ) { }

  ngOnInit(): void {
    if (this.minDate === undefined) {
      this.minDate = {
        year: 1950,
        month: 1,
        day: 1
      };
    }
    if (this.maxDate === undefined) {
      this.maxDate = this.ngbCalendar.getToday();
      this.maxDate.year -= 15;
    }
    if (this.startDate === undefined) {
      this.startDate = this.ngbCalendar.getToday();
      this.startDate.year -= 30;
    }
  }

  public get isValid(): boolean {
    return (
      this._calendar &&
      this._calendar.errors === null &&
      this.date != null &&
      this.date.hasOwnProperty('year') &&
      this.date.hasOwnProperty('month') &&
      this.date.hasOwnProperty('day')
    );
  }

  public get isTomorrowOrLater(): boolean {
    if (!this.tomorrowValidation) {
      return true;
    }
    return (
      this._calendar &&
      this._calendar.errors === null &&
      this.date != null &&
      this.date.hasOwnProperty('year') &&
      this.date.hasOwnProperty('month') &&
      this.date.hasOwnProperty('day') &&
      new Date(this.date.year, this.date.month - 1, this.date.day) >= this.tomorrow
    );
  }

  public get tomorrow(): Date {
    let today: Date = new Date();
    let tomorrow: Date = new Date(today);
    tomorrow.setDate(tomorrow.getDate() + 1);
    tomorrow.setHours(0,0,0,0);

    return tomorrow;
  }

  // method for disabling weekends
  public isDisabled(date: NgbDateStruct) {
    const d: Date = new Date(date.year, date.month - 1, date.day);
    return (d.getDay() === 0 || d.getDay() === 6);
  }
}
