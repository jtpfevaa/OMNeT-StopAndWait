/*
 * source.cc
 *
 *  Created on: 4 dic. 2019
 *      Author: Adrian
 */
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <random>
#include "custom_packet_m.h"

using namespace omnetpp;

class Source : public cSimpleModule
{
    private:
        double lambda = 2;
        int samples = 100;
        int sequenceNumber = 0;
    protected:
        virtual void initialize() override;
        virtual std::vector<double> getDepartures(double lambda, int samples);
        virtual CustomPacket* getPacket();
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Source);

void Source::initialize() {
    // Get departure times, generate packets and schedule them
    std::vector<double> departures = getDepartures(lambda, samples);
    for(int i = 0; i < departures.size(); i++) {
        // Scheduled packets will arrived to the same module at
        // departures[i]
        CustomPacket *pkt = getPacket();
        scheduleAt(departures[i], pkt);
    }
}

std::vector<double> Source::getDepartures(double lambda, int samples) {
    // Generates an exponential distribution with SAMPLES length
    // and using LAMDBA.
    std::uniform_real_distribution<double> randomReal(0.0, 1.0);
    std::default_random_engine generator(time(NULL));
    double randomNumber = ((double) rand() / (RAND_MAX));
    std::vector<double> departures(samples);
    for(int i = 0; i < departures.size(); i++) {
        double randomNumber = randomReal(generator);
        double number = (-1/lambda) * log(randomNumber);
        if (i != 0)
            departures[i] = departures[i - 1] + number;
        else
            departures[i] = number;
    }
    return departures;
}

CustomPacket* Source::getPacket() {
    std::string packetName = "packet::" + std::to_string(getId()) + "::" + std::to_string(sequenceNumber);
    char *charPacketName = new char[packetName.length() + 1];
    strcpy(charPacketName, packetName.c_str());
    CustomPacket *pkt = new CustomPacket(charPacketName);
    pkt -> setBitLength(1024);
    pkt -> setFromSource(true);
    pkt -> setKind(1);
    pkt -> setSequenceNumber(sequenceNumber);
    pkt -> setOrigin(getId());
    sequenceNumber++;
    return pkt;
}

void Source::handleMessage(cMessage *msg) {
    // Send scheduled packet
    CustomPacket *pkt = check_and_cast<CustomPacket *> (msg);
    send(pkt, "outPort");
}
