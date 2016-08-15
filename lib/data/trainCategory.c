
#include <stdio.h>
#include <stdlib.h>
#include <networkrail/timetable/lookupTable.h>
#include <limits.h>

struct table {
    char *k;
    char *label;
};

#define SIZE 54

static const struct table data[] = {
    // Ordinary Passenger Trains
    {"OL", "London Underground/Metro Service"},
    {"OU", "Unadvertised Ordinary Passenger"},
    {"OO", "Ordinary Passenger"},
    {"OS", "Staff Train"},
    {"OW", "Mixed"},
    // Express Passenger Trains
    {"XC", "Channel Tunnel"},
    {"XD", "Sleeper (Europe Night Services)"},
    {"XI", "International"},
    {"XR", "Motorail"},
    {"XU", "Unadvertised Express"},
    {"XX", "Express Passenger"},
    {"XZ", "Sleeper (Domestic)"},
    // Buss & Ship
    {"BR", "Bus - Replacement"},
    {"BS", "Bus - WTT Service"},
    {"SS", "Ship"},
    // Empty coaching stock trains
    {"EE", "Empty Coaching Stock"},
    {"EL", "Empty Coaching Stock (London Underground/Metro)"},
    {"ES", "Empty Coaching Stock (Staff)"},
    // Parcels/Postal
    {"JJ", "Postal"},
    {"PM", "Post Office Controlled Parcels"},
    {"PP", "Parcels"},
    {"PV", "Empty NPCSS"},
    // Departmental
    {"DD", "Departmental"},
    {"DH", "Civil Engineer"},
    {"DI", "Mechanical & Electrical Engineer"},
    {"DQ", "Stores"},
    {"DT", "Test"},
    {"DY", "Signal & Telecommunications Engineer"},
    // Light locomotives
    {"ZB", "Locomotive & Brake Van"},
    {"ZZ", "Light Locomotive"},
    // Railfreight
    {"J2", "RfD Automotive (Components)"},
    {"H2", "RfD Automotive (Vehicles)"},
    {"J6", "RfD Building Materials (UK Contracts)"},
    {"J5", "RfD Chemicals (UK Contracts)"},
    {"J3", "RfD Edible Products (UK Contracts)"},
    {"J9", "RfD Freightliner (Contracts)"},
    {"H9", "RfD Freightliner (Other)"},
    {"H8", "RfD European"},
    {"J8", "RfD General Merchandise (UK Contracts)"},
    {"J4", "RfD Industrial Materials (UK Contracts)"},
    // Trainload Freight
    {"A0", "Coal (Distributive)"},
    {"E0", "Coal (Electricity)"},
    {"B0", "Coal (Other) & Nuclear"},
    {"B1", "Metals"},
    {"B4", "Aggregates"},
    {"B5", "Domestic & Industrial Waste"},
    {"B6", "Building Materials (TLF)"},
    {"B7", "Petroleum Products"},
    // Railfreight Distribution (Channel Tunnel)
    {"H0", "RfD European Channel Tunnel (Mixed Business)"},
    {"H1", "RfD European Channel Tunnel Intermodal"},
    {"H3", "RfD European Channel Tunnel Automotive"},
    {"H4", "RfD European Channel Tunnel Contract Services"},
    {"H5", "RfD European Channel Tunnel Haulmark"},
    {"H6", "RfD European Channel Tunnel Joint Venture"}
};

int ttref_parse_trainCategory(char a, char b) {
    if (!(a == b == ' '))
        for (int i = 0; i < SIZE; i++)
            if (data[i].k[0] == a && data[i].k[1] == b)
                return i + 1;
    return 0;
}

char *ttref_print_trainCategory(int id) {
    return id < 1 || id > SIZE ? NULL : data[id - 1].label;
}
