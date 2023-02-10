# 
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
#  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
#  License for the specific language governing permissions and limitations under
#  the License.

# script collects all service logs files and zips them into an archive and then removes the source files.
set -e
zip -r service_logs.zip ./scheduling_service/logs ./intersection_model/logs ./tsc_client_service/logs ./message_services/logs ./signal_opt_service/logs
sudo rm ./scheduling_service/logs/* ./intersection_model/logs/* ./tsc_client_service/logs/* ./message_services/logs/* ./signal_opt_service/logs/*
