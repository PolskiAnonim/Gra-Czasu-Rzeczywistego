#include "Queue.hpp"

Queue& Queue::GetInstance() {
    static Queue instance;
    return instance;
}


void Queue::AddClient(client_t & client) {
    bool fl=false;
    for (int i=0;i<clients.size();i++)  //add to existing queue
        if (clients[i].size()<4) {
            clients[i].push_back(client);
            fl= true;
            break;
        }
    if (!fl) {  //add new queue and client to it
        clients.push_back(vector<client_t>());
        clients[clients.size()-1].push_back(client);
        timer.push_back(time(NULL));
    }
}

void Queue::TryResetTimer(int& t) {
        short nr = 0; //clients without player color and name information
        for (int i = 0; i < clients[t].size(); i++)
            if (clients[t][i].queue == nullptr)
                nr++;
        //reset queue timer if there are less than 3 players or not all have color and name
        if (clients[t].size() < 2 || nr != 0)
            timer[t] = time(NULL);
        if (clients[t].size()==4 && nr==0)
            timer[t]=timer[t]-30;
}

//Get client color and name after connection
void Queue::ColorAndName(int& t) {
        TryResetTimer(t);
        //Clients without information about name and color
        vector<int> emptyclientsind;
        vector<pollfd> set;    //Vector with descriptors because why not, it's easier
        //Get sockets for clients without name and color
        for (int c = clients[t].size() - 1; c >= 0; c--) { //from the end to delete safely
            if (clients[t][c].queue == nullptr) {
                set.push_back(pollfd{clients[t][c].socket, POLLIN | POLLHUP});
                emptyclientsind.push_back(c);
            }
        }
        //if there are clients without name and color
        if (!emptyclientsind.empty()) {
            int nfds = set.size();
            int res = poll(set.data(), nfds, 0);
            if (res <= 0) //timeout or something really wrong
                return;
            else {  //res>0
                //Get color and name information
                for (int c = 0; c < set.size(); c++) {
                    auto *queue = new NetQueue();
                    if (set[c].revents & POLLIN) {
                        if (ServerSocketHandling::GetPacketHeader(set[c].fd) ==
                            NETQUEUEPLAYER) //Check for trash in buffer
                        {
                            ServerSocketHandling::RecvQueue(set[c].fd, *queue);
                            clients[t][emptyclientsind[c]].queue = queue;  //Assign color and name to client
                        } else    //trash in buffer - there is no color and name... it shouldn't occur
                        {
                            delete queue;
                            close(clients[t][emptyclientsind[c]].socket);
                            clients[t].erase(clients[t].begin() + emptyclientsind[c]);
                            if (clients[t].empty()) {
                                timer.erase(timer.begin()+t);
                                clients.erase(clients.begin() + t--);
                            }
                            cout<<"Client deleted"<<endl;
                        }
                    } else if (set[c].revents & POLLHUP) {
                        delete queue;
                        close(clients[t][emptyclientsind[c]].socket);
                        clients[t].erase(clients[t].begin() + emptyclientsind[c]);
                        if (clients[t].empty()) {
                            timer.erase(timer.begin()+t);
                            clients.erase(clients.begin() + t--);
                        }
                        cout<<"Client deleted"<<endl;
                    }
                } //end for
            }   //end if event on descriptor
        }  //end if !emptyclientsind
} //end ColorAndName

void Queue::CheckStartNewGame(int& t) {

    if (clients[t].size() > 2 &&  time(NULL)-timer[t] > 30) {
        Lobby::GetInstance().AddGame(clients[t]); //start new game in new thread
        timer.erase(timer.begin()+t);
        clients.erase(clients.begin()+t--);

    }
}

void Queue::SendQueueState(int& t) {
    vector<int> tosendind;
    //Sockets to send queue
    vector<pollfd> set;
    for (int c = clients[t].size()-1; c >=0 ; c--) {  //to delete safely
        if (clients[t][c].queue != nullptr) {
            set.push_back(pollfd{clients[t][c].socket, POLLOUT | POLLHUP});
            tosendind.push_back(c);
        }
    }
    //if there are clients waiting in queue
    if (!tosendind.empty()) {

        int nfds = set.size();
        int res = poll(set.data(), nfds, 0);

        if (res <= 0) //timeout or something really wrong
            return;
        else {  //res>0
            for (int c = 0; c < set.size(); c++) {
                if (set[c].revents & POLLOUT) {
                    //Send queue and timer
                    ServerSocketHandling::SendQueue(set[c].fd, clients[t]);
                    char chrmsg[2] = {'T', static_cast<char>(clients[t].size() < 3 ? 99 : 30 - (time(NULL) - timer[t]))};
                    string mhelper = chrmsg;
                    NetMessage message(mhelper);
                    int r=ServerSocketHandling::SendNetMessage(set[c].fd, message);
                    if (r<=0)
                    {
                        delete clients[t][tosendind[c]].queue;
                        close(clients[t][tosendind[c]].socket);
                        clients[t].erase(clients[t].begin() + tosendind[c]);
                        if (clients[t].empty()) {
                            timer.erase(timer.begin()+t);
                            clients.erase(clients.begin() + t--);
                        }
                        cout<<"Client deleted"<<endl;
                    }
                } else if (set[c].revents & POLLHUP) {
                    delete clients[t][tosendind[c]].queue;
                    close(clients[t][tosendind[c]].socket);
                    clients[t].erase(clients[t].begin() + tosendind[c]);
                    if (clients[t].empty()) {
                        timer.erase(timer.begin()+t);
                        clients.erase(clients.begin() + t--);
                    }
                    cout<<"Client deleted"<<endl;
                }   //end events
            }   //end for
        } //end checking results
    }   //end if not empty
}   //end function


//Start new game or send inforamtion about waiting
void Queue::RunQueue() {
    //For all queues
    for (int t=0;t<Queue::GetInstance().timer.size();t++) {
        Queue::GetInstance().ColorAndName(t);
        if (Queue::GetInstance().timer.empty())
            break;
        Queue::GetInstance().CheckStartNewGame(t);
        if (Queue::GetInstance().timer.empty())
            break;
        Queue::GetInstance().SendQueueState(t);
    }
}