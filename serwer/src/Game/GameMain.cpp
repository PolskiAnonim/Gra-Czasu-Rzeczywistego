#include "GameMain.hpp"
GameInfo::GameInfo(vector<client_t> cli) {
        clients=vector<pair<short,client_t>>();
        for (short i=0;i<cli.size();i++)
            clients.push_back(make_pair(i+1,cli[i]));
}



void GameInfo::ClientSynchronize(short phase) {
    switch (phase) {
        case 1:
            recvlatch->arrive_and_wait();
            break;
        case 2:
            validationlatch->arrive_and_wait();
            break;
        case 3:
            sendlatch->arrive_and_wait();
            break;
        case 4:
            cleanlatch->arrive_and_wait();
            break;
        default:    //if this will run
            cout<<"I HATE C"<<endl;
    }
}



void GameInfo::MainWaitAndSynchronize(short phase) {
    switch (phase) {
        case 1:
            recvlatch->arrive_and_wait();
            delete cleanlatch;
            cleanlatch= new latch(clients.size()+1);
            break;
        case 2:
            validationlatch->arrive_and_wait();
            delete recvlatch;
            recvlatch=new latch(clients.size()+1);
            break;
        case 3:
            sendlatch->arrive_and_wait();
            delete validationlatch;
            validationlatch=new latch(clients.size()+1);
            break;
        case 4:
            cleanlatch->arrive_and_wait();
            ValidatePositionsAndKillPlayers();
            delete sendlatch;
            sendlatch=new latch(clients.size()+1);
            break;
        default:    //if this will run
            cout<<"I HATE C"<<endl;
    }
}

void GameInfo::ValidatePositionsAndKillPlayers() {
    //Send information about killed players
    for (auto & client : clients)
    {
        for (auto & i : invalid) {
             char msg[3]={'K',static_cast<char>(i.first),static_cast<char>(i.second)};
             string help(msg);
             NetMessage message =NetMessage(help);
             //if message could not be sent player thread will discover this so there is no problem if this function
             //don't look at result
             ServerSocketHandling::SendNetMessage(client.second.socket, NetMessage(message));
        }
    }

    for (auto& inv: invalid)
    {
        //Delate thread
        clients_threads.erase(inv.first);   //jthread - join in deconstructor + thread ended/can end if stopped earlier by breaking loop
        //Delete client
        for (int c=0;c<clients.size();c++)  //find client id with killed player id
        {
            if (clients[c].first==inv.first) {
                close(clients[c].second.socket); //close socket
                clients.erase(clients.begin() + c); //erase client - NetQueue deleted in player creation
                break;
            }

        }
        //Delete player
        players.erase(inv.first);
        //Fix latches
        recvlatch->count_down();
        validationlatch->count_down();
        sendlatch->count_down();   //it is probably useless, but it works fine, so it can stay
        //I don't want to touch this and break something...
    }
    //Clear invalid vector for use in next loop iteration
    invalid.clear();
    //Check for game end
    CheckForExitGame();
}

void GameInfo::CheckForExitGame() {
    if (clients_threads.size()==1) {    //there is only 1 player - winner
        NetMessage message("W");
        ServerSocketHandling::SendNetMessage(clients[0].second.socket, message);
    }   //client close connection and then player thread see POLLHUP and change status to killed - in next iteration
    if (clients_threads.empty()) {
        //Clean memory
        delete validationlatch;
        delete  sendlatch;
        delete recvlatch;
        delete cleanlatch;
        pthread_exit(EXIT_SUCCESS); //Exit game
    }
}

void GameInfo::Game(vector<client_t> clients_vector) {
    cout<<"Game created"<<endl;
    //Create class with shared variables for players threads use
    GameInfo gameinfo(clients_vector);

    clients_vector.clear();    //Useless outside the class

    //Send message about game start to all clients/players
    for(int l=0;l<2;l++) {  //well in first iteration it creates players for all clients and in second it sends packets about all players
        for (int i = 0; i < gameinfo.clients.size(); i++) {
            if (l==0)
                gameinfo.players[gameinfo.clients[i].first] = PlayerHandling::createPlayer(gameinfo.clients[i]);
            if (l==1) {
                ServerSocketHandling::SendPlayerCreate(gameinfo.clients[i].second.socket, gameinfo.players,
                                                           gameinfo.clients[i].first);  //looking for result after this could
                // break client so unfortunetly killing unresponsive player must wait and get its own thread that could kill it
            }
        }
    }
    //Set all latches (all clients/players+1 for this thread - game main thread)
    gameinfo.sendlatch=new latch(gameinfo.clients.size()+1);
    gameinfo.validationlatch=new latch(gameinfo.clients.size()+1);
    gameinfo.recvlatch=new latch(gameinfo.clients.size()+1);
    gameinfo.cleanlatch=new latch(gameinfo.clients.size()+1);
    //Create all threads with pointer to gameinfo class - with shared variables and client information
    for (auto & client : gameinfo.clients) {
        gameinfo.clients_threads[client.first]=jthread(ClientThread::Thread,&gameinfo,client);
    }

    //"Endless loop" - at least untill all players are dead
    auto start=chrono::steady_clock::now();
    while (true)
    {
        //One iteration - 16,67 ms - 60 Hz
        while(std::chrono::duration_cast<std::chrono::microseconds >(chrono::steady_clock::now() - start).count()<16667 );
        start=chrono::steady_clock::now();
        //Synchronization for receiving updates from players
        gameinfo.MainWaitAndSynchronize(1);
        //Synchronization for validating players
        gameinfo.MainWaitAndSynchronize(2);
        //Synchronization for sending players updated positions
        gameinfo.MainWaitAndSynchronize(3);
        //Synchronization for cleanup after iteration
        gameinfo.MainWaitAndSynchronize(4);
    }
}








