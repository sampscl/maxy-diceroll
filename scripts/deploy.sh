#!/bin/bash
if [[ `id -u` != "0" ]] ; then
  echo "You must be root to run this script"
  exit -1
fi

scriptdir=`realpath $0`
scriptdir=`dirname $scriptdir`
devdir=`realpath ${scriptdir}/../`

appuser=maxy_diceroll
appdir=/home/${appuser}
version="1.0.0"

echo "Building UI"
wd=`pwd`
cd ${scriptdir}/../ui
yarn install || exit $?
yarn run build || exit $?
cd ${wd}

echo "Deploying app from ${devdir} to ${appdir}..."

echo "Stopping services..."
systemctl disable maxy_diceroll.service
systemctl stop maxy_diceroll.service

# appuser owns all
echo "User account..."
useradd -M -s /usr/sbin/nologin ${appuser}
mkdir -p ${appdir}
chown -R ${appuser}:${appuser} ${appdir}

echo "Installing files..."
rm -f ${appdir}/maxy_diceroll
rm -rf ${appdir}/maxy_diceroll_ui

cp ${devdir}/build/src/maxy_diceroll ${appdir}/maxy_diceroll
cp -r ${devdir}/ui/build/ ${appdir}/maxy_diceroll_ui/

echo "Installing maxy_diceroll service file..."

rm -f ${appdir}/maxy_diceroll.service
cat <<EOT >> ${appdir}/maxy_diceroll.service
[Unit]
Description=Diceroll TO THE MAX
After=network-online.target

[Service]
Environment="SHELL=/bin/bash"
User=${appuser}
WorkingDirectory=${appdir}
ExecStart=${appdir}/maxy_diceroll --ui-dir=${appdir}/maxy_diceroll_ui/

[Install]
WantedBy=multi-user.target
EOT

chmod 640 ${appdir}/maxy_diceroll.service
chmod 770 ${appdir}/maxy_diceroll

echo "Setting ownership to ${appuser}"
chown -R ${appuser}:${appuser} ${appdir}

echo "Setting privileged port bind cap..."
setcap 'cap_net_bind_service=+eip' ${appdir}/maxy_diceroll

echo "Enabling maxy_diceroll service"
systemctl enable ${appdir}/maxy_diceroll.service

echo "Edit ${appdir}/maxy_diceroll.service and then run \"sudo systemctl start maxy_diceroll.service\""
echo "Done!"
