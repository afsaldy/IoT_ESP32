function doGet(e) {
  var ss = SpreadsheetApp.openById("isi id gsheet"); 
  var sheet = ss.getSheets()[0];
  
  // Ambil parameter dari ESP32
  var waktu = new Date(); // timestamp otomatis
  var suhu = e.parameter.suhu;
  var kelembaban = e.parameter.kelembaban;
  var tekanan = e.parameter.tekanan;
  var angin = e.parameter.angin;
  var pm1 = e.parameter.pm1;
  var pm25 = e.parameter.pm25;
  var pm10 = e.parameter.pm10;
  var co = e.parameter.co;
  var nh3 = e.parameter.nh3;
  var no2 = e.parameter.no2;
  var ch4 = e.parameter.ch4;
  var ozon = e.parameter.ozon;
  
  // Simpan ke baris baru
  sheet.appendRow([
    Utilities.formatDate(waktu, "Asia/Jakarta", "yyyy-MM-dd HH:mm:ss"),
    suhu,
    kelembaban,
    tekanan,
    angin,
    pm1,
    pm25,
    pm10,
    co,
    nh3,
    no2,
    ch4,
    ozon
  ]);
  
  return ContentService.createTextOutput("OK");
}
