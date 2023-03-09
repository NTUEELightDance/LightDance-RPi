USERNAME=$1
WORKSPACE="${2/#\~/$HOME}"

cp system/player.service.template system/player.service
sed -i "s!<USERNAME>!${USERNAME}!g" system/player.service
sed -i "s!<WORKSPACE>!${WORKSPACE}!g" system/player.service

cp system/client.service.template system/client.service
sed -i "s!<USERNAME>!${USERNAME}!g" system/client.service
sed -i "s!<WORKSPACE>!${WORKSPACE}!g" system/client.service
