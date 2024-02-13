#include "ClientThread.hpp"

void ClientThread::PhaseGetPlayerUpdate(GameInfo * gameinfo,int &csocket, short &id) {
        //Wait for update
        pollfd pfd[1]={csocket,POLLIN | POLLHUP};
        int res=poll(pfd,1,9);
        if (res<0) //something went reaaaaaally wrong
        {
            gameinfo->invalid.emplace_back(id, 99);
            killed=true;
            return;
        }
        else if (res==0)    //timeout - lost update
        {
            return;     //return without updating timer
        }
        else {  //res=1 - event occured
            if (pfd[0].revents & POLLHUP)   //connection lost
            {
                gameinfo->invalid.emplace_back(id, 99);
                killed=true;
                return;
            }
            else if (pfd[0].revents & POLLIN)   //there is packet to read
            {
                if (ServerSocketHandling::GetPacketHeader(csocket) == NETUPDATEPLAYER) { //check if buffer has good packet
                    //read packet
                    NetPlayerUpdatePacket packet;
                    ServerSocketHandling::RecvPlayerUpdate(csocket, packet);  //it's ready for read, and it's SCTP
                    //update player ignore memory - player upadate only itself
                    PlayerHandling::updatePlayer(gameinfo->players[id], packet);
                    //reset timer
                    timer=time(NULL);
                }
                else return; //trash in buffer
            }
        }
}

void ClientThread::PhaseValidatePlayer(GameInfo * gameinfo,int &socket, short &id)
{
    //validate moves - find killed
    pair<bool, int> valid;
    valid = PlayerHandling::validatePlayer(gameinfo->players, id);
    if (!valid.first) {
        gameinfo->invalid.emplace_back(id, valid.second);
        killed=true;
        return;
    }
}

void ClientThread::PhaseSendPlayerUpdate(GameInfo * gameinfo,int &csocket, short &id)
{
    pollfd pfd[1]={csocket,POLLOUT | POLLHUP};
    int res=poll(pfd,1,9);
    if (res<0)  //something went reaaaaaaaally wrong
    {
        gameinfo->invalid.emplace_back(id, 99);
        killed=true;
        return;
    }
    else if (res==0)    //timeout
        return;         //packet lost
    else {
        if (pfd[0].revents & POLLHUP)   //connection lost
        {
            gameinfo->invalid.emplace_back(id, 99);
            killed=true;
            return;
        }
        else if (pfd[0].revents & POLLOUT) {    //packet can be sent
            //get new positions of other players
            vector<NetPlayerUpdatePacket> otherplayers;
            otherplayers.reserve(gameinfo->players.size());
            for (auto &player: gameinfo->players) {
                if (player.first != id)
                    otherplayers.push_back(PlayerHandling::createUpdatePacket(player.second, player.first));
            }
            //send positions
            int r=ServerSocketHandling::SendPlayerUpdate(csocket, otherplayers);
            if (r<=0)
            {
                gameinfo->invalid.emplace_back(id, 99);
                killed=true;
                return;
            }
            //timer=time(NULL);
        }

    }
}

void ClientThread::Thread(GameInfo *gameinfo, pair<short, client_t> client) {
    //set timer and bool - thread variables
    timer=time(NULL);
    killed=false;
//Rozgrywka
    while (true) {
        //synchronize next phase
        gameinfo->ClientSynchronize(1);
        //Get updated positions
        PhaseGetPlayerUpdate(gameinfo,client.second.socket,client.first);
        //synchronize next phase
        gameinfo->ClientSynchronize(2);
        //check for kills
        if (!killed)
            PhaseValidatePlayer(gameinfo,client.second.socket,client.first);
        //synchronize next phase
        gameinfo->ClientSynchronize(3);
        //send updates
        if (!killed)
            PhaseSendPlayerUpdate(gameinfo,client.second.socket,client.first);
        //Check if there wasn't updates for 3 seconds or more
        if (time(NULL)-timer>=3 && !killed) {
            gameinfo->invalid.emplace_back(client.first,99);
            killed=true;
        }
        //Synchronize next phase
        gameinfo->ClientSynchronize(4);
        //if player was killed end thread
        if (killed)
            break;
    }
    cout<<"Player killed"<<endl;
}




