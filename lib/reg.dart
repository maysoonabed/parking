import 'dart:async';

import 'package:flutter/material.dart';
import 'package:http/http.dart' show get;
import 'dart:convert';

class Spacecraft {
  final String id;
  final String name, imageUrl, propellant;
  Spacecraft({
    this.id,
    this.name,
    this.imageUrl,
    this.propellant,
  });

  factory Spacecraft.fromJson(Map<String, dynamic> jsonData) {
    return Spacecraft(
      id: jsonData['id'],
      name: jsonData['name'],
      propellant: jsonData['propellant'],
      imageUrl:
          "http://192.168.12.2/PHP/spacecrafts/images/" + jsonData['image_url'],
    );
  }
}
