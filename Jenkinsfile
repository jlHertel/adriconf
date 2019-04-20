pipeline {
    agent any
    stages {
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
                dir('build-dir/tests') {
                    sh './runUnitTests --gtest_output=xml:gtestresults.xml'
                    sh 'awk \'{ if ($1 == "<testcase" && match($0, "notrun")) print substr($0,0,length($0)-2) "><skipped/></testcase>"; else print $0;}\' gtestresults.xml > gtestresults-skipped.xml'
                    sh 'mv gtestresults.xml gtestresults.off'
                }
            }
        }
    }
    post {
        always {
            archiveArtifacts 'build-dir/adriconf/adriconf,build-dir/tests/runUnitTests'
            junit 'build-dir/tests/gtestresults-skipped.xml'
            deleteDir()
        }
    }
}
