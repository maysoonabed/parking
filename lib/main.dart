import 'package:flutter/material.dart';
import 'package:parking/region.dart';
import 'package:parking/bus.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:http/http.dart' as http;
import 'package:http/http.dart' show get;
import 'dart:convert';
import 'dart:io';

//String ip = "192.168.1.108:8089"; //192.168.1.114
String ip = "192.168.1.114";
List<Bus> inBus;
var pasns = new Map();
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

class MyHttpOverrides extends HttpOverrides {
  @override
  HttpClient createHttpClient(SecurityContext context) {
    return super.createHttpClient(context)
      ..badCertificateCallback =
          (X509Certificate cert, String host, int port) => true;
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

DatabaseReference espRef = FirebaseDatabase.instance.reference().child('Esp');

class _MyHomePageState extends State<MyApp> {
  final databaseReference = FirebaseDatabase.instance.reference();

  String uidIn = "";
  String uidOut = "";

  chekIn(String uid) async {
    String apiurl = "https://$ip/parking/phpfiles/checkIn.php"; //10.0.0.8//
    var response = await http.post(apiurl, body: {
      'uid': uid, //get the username text
    });

    if (response.statusCode == 200) {
      var jsondata = jsonDecode(response.body);
      setState(() {
        if (jsondata['hay'] == 1) {
          String name = jsondata['name'];
          String busType = jsondata['busType'];
          String region = jsondata['region'];
          String passengers = jsondata['passengers'];
          Bus b = Bus();
          b.name = name;
          b.uid = uid;
          b.region = region;
          b.busType = busType;
          b.passengers = passengers;
          b.taken = 0;
          inBus.add(b);


          int lcd = 0;
          for (int i = 0; i < inBus.length; i++) {
            if (inBus[i].region == b.region) lcd++;
          }

          pasns[region] = lcd.toString();
        } else {
           print("no hay");
        }
        for (int i = 0; i < inBus.length; i++) print(inBus[i].uid);
      });
    } else {
      print("حدث خطأ أثناء الاتصال بالشبكة");
    }
  }


//*******************************************************************/
  void checkOut(String uid) {
    if (inBus.isNotEmpty) {
      int x = inBus.indexWhere((bus) => bus.uid == uid);
      String p = inBus[x].region;
      inBus.removeAt(x);

      int lcd = 0;
      for (int i = 0; i < inBus.length; i++) {
        if (inBus[i].region == p) lcd++;
      }
      setState(() {
        pasns[p] = lcd.toString();
      });
    }
  }

  @override
  void initState() {
    super.initState();
    setState(() {
      inBus = [];
      DatabaseReference reg =
      FirebaseDatabase.instance.reference().child('Esp/region');
      reg.onDisconnect();
      reg.remove();
      reg = null;


      DatabaseReference inout =
      FirebaseDatabase.instance.reference().child('Esp/rfid');
      inout.child('in').set(0);
      inout.child('out').set(0);
      DatabaseReference rst =
      FirebaseDatabase.instance.reference().child('Esp/reset');
      rst.set(1);
    });

    espRef.child('rfid').child('in').onValue.listen((event) {
      String inr = event.snapshot.value.toString();
      bool ch = true;
      if (inBus.isNotEmpty) {
        for (int i = 0; i < inBus.length; i++) {
          if (inBus[i].uid == inr) ch = false;
        }
      }
      if (ch == true) {
         chekIn(inr);
      }
    });

    espRef.child('rfid').child('out').onValue.listen((event) {
      checkOut(event.snapshot.value.toString());
     });
  }

  @override
  Widget build(BuildContext context) {
    //getList();
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
            body:new Center(
              //FutureBuilder is a widget that builds itself based on the latest snapshot
              // of interaction with a Future.
              child: new FutureBuilder<List<Spacecraft>>(
                future: downloadJSON(),
                    builder: (context, snapshot) {
                  if (snapshot.hasData) {
                    List<Spacecraft> spacecrafts = snapshot.data;

                    return new CustomListView(spacecrafts);
                  } else if (snapshot.hasError) {
                    return Text('${snapshot.error}');
                  }
                  //return  a circular progress indicator.
                  return new CircularProgressIndicator();
                },
              ),

            ),


        ));
  }

}



class Spacecraft {
  final String engn;
  final String arn;

  Spacecraft({
    this.engn,
    this.arn,

  });

  factory Spacecraft.fromJson(Map<String, dynamic> jsonData) {
    return Spacecraft(
      engn: jsonData['engname'],
      arn: jsonData['arname'],
        );
  }
}
class CustomListView extends StatelessWidget {
  final List<Spacecraft> spacecrafts;

  CustomListView(this.spacecrafts);

  Widget build(context) {
    return GridView.builder(
      gridDelegate: const SliverGridDelegateWithMaxCrossAxisExtent(
          maxCrossAxisExtent: 200,
          childAspectRatio: 3 / 2,
          crossAxisSpacing: 20,
          mainAxisSpacing: 20),
      itemCount: spacecrafts.length,
      itemBuilder: (context, int currentIndex) {
        int m=0;
        if (inBus.isNotEmpty) {
          for (int i = 0; i < inBus.length; i++) {
            if (inBus[i].region == spacecrafts[currentIndex].engn) m++;
          }
        }
          pasns[spacecrafts[currentIndex].engn] = m.toString();
        return createViewItem(spacecrafts[currentIndex], context);
      },
    );
  }

  Widget createViewItem(Spacecraft spacecraft, BuildContext context) {
    return new ListTile(
        title: new Card(
          elevation: 5.0,
          child: new Container(
            child:         ElevatedButton(
              style: ElevatedButton.styleFrom(primary: apBcolor),
              onPressed: () {
                 Navigator.push(
                  context,
                  MaterialPageRoute(builder: (context) => region(  spacecraft.engn, "")),
                );
              },
              child: Column(children: [
                Text(
                  " ",
                  style: TextStyle(
                    fontSize: 10,
                    fontFamily: 'ArefRuqaaR',
                    color: Colors.white,
                  ),
                ),
                Text(
                  spacecraft.arn,
                  style: TextStyle(
                    fontSize: 33,
                    fontFamily: 'ArefRuqaaR',
                    color: Colors.white,
                  ),
                ),
                Text(
                  pasns[  spacecraft.engn],
                  style: TextStyle(
                      fontSize: 20,
                       color: Colors.yellow),
                ),
              ]), //),
              //]),
            ),



          ),
        ),
        onTap: () {
         });
  }
}

//Future is n object representing a delayed computation.
Future<List<Spacecraft>> downloadJSON() async {
  final jsonEndpoint =
      "https://$ip/parking/phpfiles/GetRegions.php";

  final response = await get(jsonEndpoint);

  if (response.statusCode == 200) {
    List spacecrafts = json.decode(response.body);
    return spacecrafts
        .map((spacecraft) => new Spacecraft.fromJson(spacecraft))
        .toList();
  } else
    throw Exception('We were not able to successfully download the json data.');
}
