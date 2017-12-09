﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GearSDK_CSharpDLL.Gear_Objects
{
    public class Gear_PulseDetector_csharp
    {
        #region Attributes

        int id;
        string name;
        string pin;
        int value;

        #endregion

        #region Constructors

        public Gear_PulseDetector_csharp()
        {

        }

        public Gear_PulseDetector_csharp(int id, string name, string pin, int value)
        {
            this.id = id;
            this.name = name;
            this.pin = pin;
            this.value = value;
        }

        #endregion

        #region Gets and Sets

        public int Id { get => id; }
        public string Name { get => name; }
        public string Pin { get => pin; }
        public int Value { get => value; set => this.value = value; }

        #endregion
    }
}

namespace Json.PulseDetector
{
    public class Header
    {
        public string name { get; set; }
        public string pin { get; set; }
        public int value { get; set; }
    }

    public class Root_Header
    {
        public Header pulseDetector { get; set; }
    }

    public class Data
    {
        public string name { get; set; }
        public int value { get; set; }
    }

    public class Root_Data
    {
        public Data pulseDetector { get; set; }
    }
}