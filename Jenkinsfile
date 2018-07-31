pipeline {
    agent any
    stages {
        stage('Clean') {
            steps {
                echo 'Running clean stage'
                sh 'rm -rf build-dir/'
                sh 'rm -f *.deb'
            }
        }
        stage('Build') {
            steps {
                echo 'Running build stage'
                sh 'mkdir build-dir'
                dir('build-dir'){
                    sh 'cmake ..'
                    sh 'cmake --build . --target adriconf'
                    sh 'cmake --build . --target runUnitTests'
                    sh 'make translations'
                }
            }
        }
        stage('Test') {
            steps {
                echo 'Running test stage'
                dir('build-dir') {
                    sh './runUnitTests'
                }
            }
        }
        stage('DebPackage') {
            steps {
                sh 'mkdir -p usr/bin/'
                dir('build-dir') {
                    sh 'strip --strip-unneeded -o adriconf-stripped adriconf'
                }

                sh 'cp build-dir/adriconf-stripped usr/bin/adriconf'
                sh 'mkdir -p usr/share/locale/'

                dir('build-dir') {
                    sh 'for fn in *.gmo; do TRLANG=`echo "$fn" | cut -d"." -f1`; mkdir -p ../usr/share/locale/$TRLANG/LC_MESSAGES/; cp $fn ../usr/share/locale/$TRLANG/LC_MESSAGES/adriconf.mo; done'
                }

                sh 'binarySize=$(du -s usr/ | cut -f1); replaceString="s/__BINARY_SIZE__/"$binarySize"/"; sed -i $replaceString DEBIAN/control'
                sh 'versionStr=$(cat VERSION); sed -i "s/__VERSION__/"${versionStr}"/" DEBIAN/control'

                sh 'mkdir -p usr/share/doc/adriconf/'
                sh 'cp DEBIAN/copyright usr/share/doc/adriconf/'
                sh 'gzip -n9 DEBIAN/changelog'
                sh 'cp DEBIAN/changelog.gz usr/share/doc/adriconf/'

                sh 'fakeroot tar czf data.tar.gz usr/'
                dir('DEBIAN') {
                    sh 'fakeroot tar czf ../control.tar.gz control'
                }
                sh 'echo 2.0 > debian-binary'
                sh 'versionStr=$(cat VERSION);fakeroot ar r adriconf-$versionStr.deb debian-binary control.tar.gz data.tar.gz'

                echo 'Cleaning generated artifacts...'
                sh 'rm -rf usr/'
                sh 'rm control.tar.gz'
                sh 'rm data.tar.gz'
                sh 'rm debian-binary'
            }
        }
    }
}
