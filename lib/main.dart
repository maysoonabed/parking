import 'package:flutter/material.dart';
import 'package:parking/region.dart';
import 'package:parking/bus.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';
import 'dart:io';

String ip="192.168.1.109";

List<Bus> inBus;
List<String> strs= ["Al-Fara","Tubas", "Tammon", "Badhan","Talluza","Nasariah","Beta","Aqraba","Asira","Hawara","Salem","Rojib"];

Color apcolor = const Color(0xFF1ABC9C);
Color apBcolor = const Color(0xFF00796B);
Color iconBack = const Color(0xFF0e6655);

Color bacolor = const Color(0xFFBDBDBD);
Color ba1color = const Color(0xFFf2f1e3);
Color myG = const Color(0xFFebe9e2);

Color mypink = const Color(0xFFbc1a3a);
Color myOrange = const Color(0xFFbc4b1a);
Color myPink = const Color(0xFFbc1a8b);
Color myblue = const Color(0xFF1a8bbc);
Color mygreen = const Color(0xFF1abc4b);

const List<Color> myGradients1 = [
  Color(0xFF02100d),
  Color(0xFF05211b),
  Color(0xFF07322a),
  Color(0xFF094338),
  Color(0xFF0c5546),
  Color(0xFF0e6655),
  Color(0xFF107763),
  Color(0xFF138871),
  Color(0xFF159a7f),
  Color(0xFF18ab8e),
  Color(0xFF1abc9c), //this is my color
  Color(0xFF1ccdaa),
  Color(0xFF1fdeb9),
];
const List<Color> myGradients2 = [
  Color(0xFF1fdeb9),
  Color(0xFF1ccdaa),
  Color(0xFF1abc9c), //this is my color
  Color(0xFF18ab8e),
  Color(0xFF159a7f),
  Color(0xFF138871),
  Color(0xFF107763),
  Color(0xFF0e6655),
  Color(0xFF0c5546),
  Color(0xFF094338),
  Color(0xFF07322a),
  Color(0xFF05211b),
  Color(0xFF02100d),
];
const List<Color> myGradients3 = [
  Color(0xFF1abc74),
  Color(0xFF1abc81),
  Color(0xFF1abc8f),
  Color(0xFF1abc9c),
  Color(0xFF1abcaa),
  Color(0xFF1abcb7),
  Color(0xFF1ab4bc),
];
const List<Color> myGradients4 = [
  Color(0xFF64726f),
  Color(0xFF5c7a74),
  Color(0xFF548279),
  Color(0xFF4b8b7e),
  Color(0xFF439383),
  Color(0xFF3b9b88),
  Color(0xFF33a38d),
  Color(0xFF2aac92),
  Color(0xFF22b497),
  Color(0xFF1abc9c), //this is my color
  Color(0xFF12c4a1),
  Color(0xFF0acca6),
  Color(0xFF01d5ab),
];


class MyHttpOverrides extends HttpOverrides{
  @override
  HttpClient createHttpClient(SecurityContext context){
    return super.createHttpClient(context)
      ..badCertificateCallback = (X509Certificate cert, String host, int port)=> true;
  }
}
void main() {
  HttpOverrides.global = new MyHttpOverrides();
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  MyApp({Key key, this.title}) : super(key: key);

  final String title;

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyApp> {
  final databaseReference = FirebaseDatabase.instance.reference();
  DatabaseReference espRef = FirebaseDatabase.instance.reference().child('Esp');

  double ledOn = 0;

  double ledOff = 0;
  String uidIn = "";
  String uidOut = "";


  chekIn(String uid) async{
    String apiurl = "https://$ip/parking/phpfiles/checkIn.php"; //10.0.0.8//
    var response = await http.post(apiurl, body: {
      'uid': uid, //get the username text
    });

    if (response.statusCode == 200) {
      var jsondata = jsonDecode(response.body);
      setState(() {
        if(jsondata['hay']==1) {
          String name=jsondata['name'];
          String busType=jsondata['busType'];
          String region=jsondata['region'];
          String passengers=jsondata['passengers'];
          Bus b = Bus();
          b.name=name;
          b.uid=uid;
          b.region=region;
          b.busType=busType;
          b.passengers=passengers;
          b.taken=0;
          inBus.add(b);

          _turnOnLed();
          DatabaseReference  pass = FirebaseDatabase.instance
              .reference()
              .child('Esp/region/$region/$uid');
             pass.set(0);
           int lcd=0;
          for(int i=0;i<inBus.length;i++){
            if(inBus[i].region==  b.region)
            lcd++;}
          DatabaseReference  regs = FirebaseDatabase.instance
              .reference()
              .child('Esp/lcd/$region');

          regs.set(lcd);


        } else{
          _turnOffLed();
          print("no hay");
        }
        for(int i=0;i<inBus.length;i++)
          print(inBus[i].uid);
      });
    } else {
      print("حدث خطأ أثناء الاتصال بالشبكة");
    }

  }

  void checkOut(String uid){
    if(inBus.isNotEmpty){
    int x=inBus.indexWhere((bus) => bus.uid==uid);
    String p=inBus[x].region;
    DatabaseReference  pass = FirebaseDatabase.instance
        .reference()
        .child('Esp/region/$p/$uid');
    pass.onDisconnect();
    pass.remove();
    pass = null;

    inBus.removeAt(x);

    int lcd=0;
    for(int i=0;i<inBus.length;i++){
      if(inBus[i].region==p)
        lcd++;}
    DatabaseReference  regs = FirebaseDatabase.instance
        .reference()
        .child('Esp/lcd/$p');
    regs.set(lcd);


    }}


  @override
  void initState() {
    super.initState();
    setState(() {
    inBus = [];
    DatabaseReference  reg = FirebaseDatabase.instance
        .reference()
        .child('Esp/region');
    reg.onDisconnect();
    reg.remove();
    reg = null;
    DatabaseReference  LCD = FirebaseDatabase.instance
        .reference()
        .child('Esp/lcd');
     for(int i=0;i<12;i++){
       LCD.child('${strs[i]}').set(0);
     }
    DatabaseReference  inout = FirebaseDatabase.instance
        .reference()
        .child('Esp/rfid');
    inout.child('in').set(0);
    inout.child('out').set(0);
    });

    databaseReference.child("Esp").once().then((DataSnapshot snapshot) {
      ledOn = snapshot.value['ledStatus']['ledOn'].toDouble();
      ledOff = snapshot.value['ledStatus']['ledOff'].toDouble();
      uidIn = snapshot.value['rfid']['in'].toString();
      uidOut = snapshot.value['rfid']['out'].toString();
      print(uidIn);
     });

    espRef.child('rfid').child('in').onValue.listen((event) {

      chekIn(event.snapshot.value.toString());

    });

    espRef.child('rfid').child('out').onValue.listen((event) {

      checkOut(event.snapshot.value.toString());
      _turnOnLed2();

    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
        debugShowCheckedModeBanner: false, //لإخفاء شريط depug
        home: Scaffold(
            backgroundColor: myG, //ba1color,
            appBar: AppBar(
              automaticallyImplyLeading: false,
              actions: <Widget>[
                new Container(),
              ],
              title: Center(
                child: Text(
                  "OtoBüs",
                  style: TextStyle(
                    fontSize: 25,
                    fontFamily: 'Pacifico',
                    color: Colors.white,
                  ),
                ),
              ),
              backgroundColor: apcolor,
            ),
            body: GridView.count(
                crossAxisCount: 3,
                crossAxisSpacing: 10.0,
                mainAxisSpacing: 10.0,
                children: List.generate(choices.length, (index) {
                  return SelectCard(choice: choices[index]);
                }))));
  }

  void _turnOnLed() {
    databaseReference.child("Esp/ledStatus").update({
      'ledOn': 1,
    });
    setState(() {
      ledOn = 1;
     });
  }

  void _turnOffLed() {
    databaseReference.child("Esp/ledStatus").update({
      'ledOn': 0,
    });

    setState(() {
      ledOn = 0;
     });
  }

  void _turnOnLed2() {
    databaseReference.child("Esp/ledStatus").update({
      'ledOff': 1,
    });
    setState(() {
      ledOff = 1;
    });
  }

  void _turnOffLed2() {
    databaseReference.child("Esp/ledStatus").update({
      'ledOff': 0,
    });

    setState(() {
      ledOff = 0;
    });
  }


}

class Choice {
  const Choice({this.title,this.name}); //, this.icon
  final String title;
  final String name;
  //final IconData icon;
}

const List<Choice> choices = const <Choice>[
  const Choice(title: 'طوباس',name:"Tubas"),
  const Choice(title: 'طمون',name:"Tammon"),
  const Choice(title: 'الفارعة',name:"Al-Fara"),
  const Choice(title: 'الباذان',name:"Badhan"),
  const Choice(title: 'طلوزة',name:"Talluza"), //, icon: Icons.camera_alt
  const Choice(title: 'النصارية',name:"Nasariah"),
  const Choice(title: 'بيتا',name:"Beta"),
  const Choice(title: 'عقربة',name:"Aqraba"),
  const Choice(title: 'عصيرة ',name:"Asira"),
 /* const Choice(title: 'بيت دجن',name:"Tubas"),
  const Choice(title: 'بيت فوريك',name:"Tubas"),
  const Choice(title: 'دير الحطب',name:"Tubas"),*/
 /* const Choice(title: 'حوارة',name:"Hawara"),
  const Choice(title: 'سالم',name:"Salem"),
  const Choice(title: 'روجيب',name:"Rojib"),*/
];

class SelectCard extends StatelessWidget {
  const SelectCard({Key key, this.choice}) : super(key: key);
  final Choice choice;

  @override
  Widget build(BuildContext context) {
    return //Card(
        //color: apcolor,
        //child: //Center(
        //child: Column(
        //crossAxisAlignment: CrossAxisAlignment.center,
        //children: <Widget>[
        //Expanded(child:Icon(choice.icon, size: 50.0, color: color.white)),
        ElevatedButton(
      style: ElevatedButton.styleFrom(primary: apBcolor),
      onPressed: () {
        //Navigator.push(context, )
        Navigator.push(
          context,
          MaterialPageRoute(builder: (context) => region(choice.name)),
        );
      },
      child: Text(
        choice.title,
        style: TextStyle(
          fontSize: 29,
          fontFamily: 'ArefRuqaaR',
          color: Colors.white,
        ),
      ), //),
      //]),
    );
  }
}
