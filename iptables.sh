sudo apt install iptables

sudo iptables -A INPUT -p tcp --dport 666 -j ACCEPT
sudo iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

sudo iptables -L -n -v | grep 666

sudo apt install iptables-persistent
sudo netfilter-persistent save
