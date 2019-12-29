export default class Download {
  constructor() {
    this.lines = [];
  }

  reset() {
    this.lines = [];
  }

  addNewLine(dataset) {
    this.lines.push(dataset);
  }

  download() {
    let csv = "";
    let headings = Object.keys(this.lines[0]);
    csv += headings.join(';');
    csv += "\n";

    this.lines.forEach(function(row) {
      csv += Object.values(row).join(';');
      csv += "\n";
    });
    csv = csv.replace(/[.]/g, ',');
    var hiddenElement = document.createElement('a');
    hiddenElement.href = 'data:text/csv;charset=utf-8,' + encodeURI(csv);
    hiddenElement.target = '_blank';
    hiddenElement.download = 'data.csv';
    hiddenElement.click();
  }
}