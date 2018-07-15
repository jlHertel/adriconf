pipeline {
    agent any
    stages {
        stage('Clean') {
            steps {
                echo 'Running clean stage'
                sh 'rm -rf build-dir/'
            }
        }
        stage('Build') {
            steps {
                echo 'Running build stage'
                sh 'mkdir build-dir'
                dir('build-dir'){
                    sh 'cmake ..'
                    sh 'cmake --build . --target runUnitTests'
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
    }
}
